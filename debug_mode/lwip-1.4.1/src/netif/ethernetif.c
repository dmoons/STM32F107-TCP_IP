#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "netif/ppp_oe.h"
#include "stm32_eth.h"
#include <string.h>

//����������
#define IFNAME0 'e'
#define IFNAME1 'n'

#define  ETH_DMARxDesc_FrameLengthShift           16         //DMA������������RDES0����Ĵ���������֡���ȵ�λ��ƫ��ֵ
#define  ETH_ERROR                                ((u32)0)   //�������
#define  ETH_SUCCESS                              ((u32)1)   //�޴����

#define ETH_RXBUFNB        5  //���ջ���������
#define ETH_TXBUFNB        5  //���ͻ���������

extern u8_t MACaddr[6];                           //MAC��ַ������Ψһ��
extern ETH_DMADESCTypeDef  *DMATxDescToSet;  //��ǰDMA����������ָ�룬����̫�����ļ��ж����
extern ETH_DMADESCTypeDef  *DMARxDescToGet;  //��ǰDMA����������ָ�룬����̫�����ļ��ж����

ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB], DMATxDscrTab[ETH_TXBUFNB];                    //���ͺͽ���DMA����������
uint8_t Rx_Buff[ETH_RXBUFNB][ETH_MAX_PACKET_SIZE], Tx_Buff[ETH_TXBUFNB][ETH_MAX_PACKET_SIZE];//���ͺͽ��ջ�����

//����֡�ṹ�壬������ʹ�õ��������
typedef struct{
u32_t length;                     //֡����
u32_t buffer;                     //������
ETH_DMADESCTypeDef *descriptor;   //ָ��DMA��������ָ��
}FrameTypeDef;


//ǰ�õĺ�������
FrameTypeDef ETH_RxPkt_ChainMode(void);       //������������
u32_t ETH_GetCurrentTxBuffer(void);           //��ȡ��ǰDMA���������������ݻ�����ָ��
u32_t ETH_TxPkt_ChainMode(u16 FrameLength);   //������������
err_t ethernetif_input(struct netif *netif);  //�ϲ�ӿں���

//��ʼ������
static void low_level_init(struct netif *netif)      
{
  uint8_t i;
	
  netif->hwaddr_len = ETHARP_HWADDR_LEN;  //����MAC��ַ����

  netif->hwaddr[0] = MACaddr[0];  //����MAC��ַ��6λ����ַΨһ�������ظ�
  netif->hwaddr[1] = MACaddr[1];
  netif->hwaddr[2] = MACaddr[2];
  netif->hwaddr[3] = MACaddr[3];
  netif->hwaddr[4] = MACaddr[4];
  netif->hwaddr[5] = MACaddr[5];

  netif->mtu = 1500;   //����䵥Ԫ
  
  //������������
  //NETIF_FLAG_BROADCAST����㲥
  //NETIF_FLAG_ETHARP����ARP����
  //NETIF_FLAG_LINK_UP���ú�ӿڲ���һ����Ծ�����ӣ�Ҫ����Ӳ��У��
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
	
  //����������Ҫ��ʼ�����ͺͽ���DMA����������
  //107VCT6������ʽ�ṹ
  //����Ҫ�ȴ���DMA����������
  //DMA�������ڰ�����һ��ָ����պͷ��ͻ�������ָ�룬���ǻ�Ҫ�������պͷ��ͻ���������������
  ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);//��ʼ������DMA����������
  ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);//��ʼ������DMA����������
	
	//����DMA�����������ж�
	for(i=0; i<ETH_RXBUFNB; i++)
	{
		ETH_DMARxDescReceiveITConfig(&DMARxDscrTab[i], ENABLE);
	}
	
#if  !CHECKSUM_GEN_ICMP    //�ж��Ƿ���Ӳ��У��,�ر����У��
    //��������֡У�� 
	  for(i=0; i<ETH_TXBUFNB; i++)
    {
      ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
    }
#endif
		
	 ETH_Start();//����MAC��DMA
}

//�������ݺ���
//��һ���򵥵Ŀ�ܺ�DMA�������Ľṹ
//����˼·����
//Ҫ���͵����ݴ������Ϊ������������pubf��
//DMA��������������ָ�򻺳�����ָ�룬��������Ҳ�����˻�����
//��������Ҫ�õ���������DMA������ָ�룬��������Ҫʵ��һ��ETH_GetCurrentTxBuffer����
//���������ǽ�pbuf�����ݿ�����������
//����ʹ�õ�������дһ�������������ݵĺ���ETH_TxPkt_ChainMode
//�⼸������ST�ٷ������˻���DP83848������
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q; //��һ��pbuf���������ݴ濽���м�����
  int l = 0;      //����
  u8 *buffer =  (u8 *)ETH_GetCurrentTxBuffer();//��ȡ��ǰDMA�ڻ�����ָ�룬��Ҫ���͵����ݣ�������������
  
  for(q = p; q != NULL; q = q->next)   //����forѭ������������
  {
    memcpy((u8_t*)&buffer[l], q->payload, q->len);
	l = l + q->len;
  }

  ETH_TxPkt_ChainMode(l);//��������

  return ERR_OK;
}

//�������ݺ���
//��һ���򵥵Ŀ�ܺ�DMA�������Ľṹ
//����˼·����
//���������յ����ݣ������ڽ��ջ�����������DMA����������ָ�����ָ��
//���ǻ�Ҫʵ��һ�������������ݵĺ���ETH_TxPkt_ChainMode��ͬ����һ��ST�ṩ������
//�õ������������ݺ�����Ҫ���俽����pbuf�ṹ�У���LWipʹ��
//��������������ݿ�����pbuf�󣬽�����Ϊ��������ֵ������
static struct pbuf * low_level_input(struct netif *netif)
{
  struct pbuf *p, *q; //pҪ���ص����ݣ�q��������ʱ�����ݴ�����
  u16_t len;          //������յ�����֡�ĳ���
  int l =0;           //���ȣ�forʱ�ݴ��м�ֵ
  FrameTypeDef frame; //������
  u8 *buffer;         //���յ����ݵĵ�ַ
  
  p = NULL; //p��ָ��գ�����
  frame = ETH_RxPkt_ChainMode();//��������֡

  len = frame.length;//������֡���ȴ����len�ڴ���
  
  buffer = (u8 *)frame.buffer; //�õ���������ַ

  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);//�ڴ�ط���ռ�

  if (p != NULL)//����ɹ�
  {
    for (q = p; q != NULL; q = q->next)//����forѭ����������
    {
	  memcpy((u8_t*)q->payload, (u8_t*)&buffer[l], q->len);
      l = l + q->len;
    }    
  }

  frame.descriptor->Status = ETH_DMARxDesc_OWN; //����DMAռ��������
 
  if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)  //ͨ���ж�ETH->DMASR�Ĵ���λ7���жϽ��ջ������ɲ�����
  {
    //���ջ����������ã�if����
	ETH->DMASR = ETH_DMASR_RBUS; //������ջ����������ñ�־
    ETH->DMARPDR = 0;//ͨ��дETH->DMARPDR�Ĵ������ָ�DMA����
  }

  return p;//��������
}

//LWip���õĽ������ݺ���
//��Ҫ���ǵ�����low_level_input����
err_t ethernetif_input(struct netif *netif)
{
  err_t err;
  struct pbuf *p;

  p = low_level_input(netif);//����low_level_input������������

  if (p == NULL) return ERR_MEM;//�����ݿɶ������ش������

  err = netif->input(p, netif);//����LWipԴ�봦������
  if (err != ERR_OK) //�������ʧ��
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));//������Ϣ
    pbuf_free(p);//�ͷŵ�pbuf�ռ�
    p = NULL;
  }

  return err;
}


err_t ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
  netif->hostname = "lwip";//����
#endif 

  //��ʼ��netif����ֶ�
	netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  low_level_init(netif);

  return ERR_OK;
}

//�����������ݺ���
FrameTypeDef ETH_RxPkt_ChainMode(void)
{ 
  u32 framelength = 0;             //��������
  FrameTypeDef frame = {0,0};      //֡�ṹ����

  if((DMARxDescToGet->Status & ETH_DMARxDesc_OWN) != (u32)RESET)//���DMAռ������������
  {	
	frame.length = ETH_ERROR;   //��Ŵ������

    if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)  //������ͻ��治���ã�if����
    {
		ETH->DMASR = ETH_DMASR_RBUS; //������ջ����������ñ�־
		ETH->DMARPDR = 0;//ͨ��дETH->DMARPDR�Ĵ������ָ�DMA����
    }
	
    return frame; //����֡�ṹ
  }
  //����ϲ�if����������־��������CPUռ��
  //��Ҫ����3���ж�
  //ETH_DMARxDesc_ES�жϽ������Ƿ����������ʾû�д�����
  //ETH_DMARxDesc_LS�ж��Ƿ������һ��������
  //ETH_DMARxDesc_FS�ж��Ƿ������֡�ĵ�һ��������
  if(((DMARxDescToGet->Status & ETH_DMARxDesc_ES) == (u32)RESET) && 
     ((DMARxDescToGet->Status & ETH_DMARxDesc_LS) != (u32)RESET) &&  
     ((DMARxDescToGet->Status & ETH_DMARxDesc_FS) != (u32)RESET))  
  {      
     //�������Ļ����õ�֡����ֵ��
	 //DMA����������RDES0����Ĵ���λ16-λ29���֡����ֵ
	 //����16λ��Ȼ��Ҫ��ȥ4���Լ���CRCУ��
    framelength = ((DMARxDescToGet->Status & ETH_DMARxDesc_FL) >> ETH_DMARxDesc_FrameLengthShift) - 4;
	
	frame.buffer = DMARxDescToGet->Buffer1Addr;	//�õ�������������Buffer1Addr��ַ����ָ�������ݻ�����
  }
  else//����ϲ�if������
  {
    framelength = ETH_ERROR;//��¼�������
  }

  frame.length = framelength; //��֡����ֵ����¼��frame�ṹ���е�length��Ա


  frame.descriptor = DMARxDescToGet;//frame�ṹ���е�descriptor��Աָ��ǰ��DMA����������
  
  DMARxDescToGet = (ETH_DMADESCTypeDef*) (DMARxDescToGet->Buffer2NextDescAddr);//����ǰ����DMA������ָ�룬ָ����һ������DMA�����е�DMA������  

  return (frame);  //����֡�ṹ
}

//�����������ݺ���
u32_t ETH_TxPkt_ChainMode(u16 FrameLength)
{   
  if((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (u32)RESET)//���DMAռ������������
  {  
    return ETH_ERROR;//���ش������
  }
        
  //���if����������ʾCPUռ��������
  DMATxDescToSet->ControlBufferSize = (FrameLength & ETH_DMATxDesc_TBS1);//���÷���֡����

  DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;//ETH_DMATxDesc_LS��ETH_DMATxDesc_FS��1����ʾ֡�д���ˣ���һ�������һ���ֿ�

  DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;//����������DMAʹ��

  if ((ETH->DMASR & ETH_DMASR_TBUS) != (u32)RESET)//������ͻ��治���ã�if����
  {
    ETH->DMASR = ETH_DMASR_TBUS;//������ͻ��治���ñ�־
    ETH->DMATPDR = 0;//дETH->DMATPDR�Ĵ���������ظ���������
  }

  DMATxDescToSet = (ETH_DMADESCTypeDef*) (DMATxDescToSet->Buffer2NextDescAddr);//����ǰ����DMA������ָ�룬ָ����һ������DMA�����е�DMA������     


  return ETH_SUCCESS;   //���سɹ�����
}

//��ȡ����DMA�������µĻ�����
u32_t ETH_GetCurrentTxBuffer(void)
{ 
  return (DMATxDescToSet->Buffer1Addr);   //�õ�DMA��������Buffer1Addr��ַ��
}

