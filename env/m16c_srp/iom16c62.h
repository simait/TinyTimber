#ifndef IOM16C63_H_
#define IOM16C63_H_

/****************************************************************
KPIT Cummins Infosystems Ltd, Pune, India.  27-June-2005.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************/
/****************************************************************/
/*      M16C/60 62A Include File                                */
/****************************************************************/


/*------------------------------------------------------
    Processor mode register 0
------------------------------------------------------*/
union st_pm0 {               /* union PM0    	   					*/
  struct {             		 /* Bit  Access 		   				*/
	 unsigned char 	PM0_0:1; /* Processor mode bit 0 				*/
	 unsigned char 	PM0_1:1; /* Processor mode bit 1 				*/
	 unsigned char 	PM0_2:1; /* R/W mode select bit  				*/
	 unsigned char 	PM0_3:1; /* Software reset bit					*/
	 unsigned char 	PM0_4:1; /* Multiplexed bus space select bit 0	*/
	 unsigned char 	PM0_5:1; /* Multiplexed bus space select bit 1	*/
	 unsigned char 	PM0_6:1; /* Port P40 to P43 function select bit */
	 unsigned char 	PM0_7:1; /* BCLK output disable bit		      	*/
  } BIT;   		     		 /*        						    	*/
	unsigned  char BYTE;     /*  Byte Access 						*/
};                           /* 						            */

/*------------------------------------------------------
    Processor mode register 1
------------------------------------------------------*/
union st_pm1 {              /* union PM1    	   					*/
  struct {            		/* Bit  Access 		   					*/
	 unsigned char 		:1; /* Reserved bit 						*/
	 unsigned char 		:1; /* Nothing assigned 					*/
	 unsigned char 		:1; /* Nothing assigned  					*/
	 unsigned char PM1_3:1; /* PM13(Internal reserved area expansion bit 0*/
	 unsigned char 		:1; /* Reserved bit 						*/
	 unsigned char 		:1; /* Reserved bit 						*/
	 unsigned char 		:1; /* Reserved bit  						*/
	 unsigned char PM1_7:1; /* PM17 - Wait bit				    	*/
  } BIT;   		    		/*        						    	*/
	 unsigned char BYTE;    /*  Byte Access 						*/
};                          /* 						            	*/

/*------------------------------------------------------
    System clock control register 0
------------------------------------------------------*/
union st_cm0 {               /* union CM0    	   					*/
  struct {             		 /* Bit  Access 		   				*/
	 unsigned char 	CM0_0:1; /* Clock output function select bit 	*/
	 unsigned char 	CM0_1:1; /* Clock output function select bit 	*/
	 unsigned char 	CM0_2:1; /* WAIT peripheral function clock stop bit */
	 unsigned char 	CM0_3:1; /* Xcin-Xcout drive capacity select bit*/
	 unsigned char 	CM0_4:1; /* Port Xc select bit 					*/
	 unsigned char 	CM0_5:1; /* Main clock stop bit					*/
	 unsigned char 	CM0_6:1; /* Main clock division select bit 0 	*/
	 unsigned char 	CM0_7:1; /* System clock select bit 			*/
  } BIT;   		     		 /*        						    	*/
	unsigned char BYTE;      /*  Byte Access 						*/
};                           /* system clock control register  0    */


/*------------------------------------------------------
    System clock control register 1
------------------------------------------------------*/
union st_cm1 {               /* union CM1    	   					*/
  struct {             		 /* Bit  Access 		   				*/
	 unsigned char 	CM1_0:1; /* All clock stop control bit 			*/
	 unsigned char 		 :1; /* Reserved bit always set to 0		*/
	 unsigned char 	 	 :1; /* Reserved bit always set to 0		*/
	 unsigned char 		 :1; /* Reserved bit always set to 0		*/
	 unsigned char 		 :1; /* Reserved bit always set to 0		*/
	 unsigned char 	CM1_5:1; /* Xin-Xouts drive capacity select bit */
	 unsigned char 	CM1_6:1; /* Main clock division select bit 1 	*/
	 unsigned char 	CM1_7:1; /* Main clock division select bit 1 	*/
  } BIT;   		     		 /*        						    	*/
	unsigned char BYTE;      /*  Byte Access 						*/
};                           /* system clock control register 1     */


/*------------------------------------------------------
    Chip select control register
------------------------------------------------------*/
union st_csr {               /* union CSR    	   					*/
   struct {             	 /* Bit  Access 		   				*/
	 unsigned char	CS0 :1;  /* CS0~ output enable bit				*/
	 unsigned char	CS1 :1;  /* CS1~ output enable bit 				*/
	 unsigned char	CS2 :1;  /* CS2~ output enable bit 				*/
	 unsigned char	CS3 :1;  /* CS3~ output enable bit 				*/
	 unsigned char	CS0W:1;  /* CS0~ wait bit 						*/
	 unsigned char	CS1W:1;  /* CS1~ wait bit 						*/
	 unsigned char	CS2W:1;  /* CS2~ wait bit 						*/
	 unsigned char	CS3W:1;  /* CS3~ wait bit 						*/
   } BIT;   		         /*        						    	*/
	 unsigned char BYTE;     /* Byte Access 						*/
};                           /* Chip select control register		*/

/*------------------------------------------------------
    Address match interrupt enable register
------------------------------------------------------*/
union st_aier {               /* union AIER    	   					*/
   struct {              	  /* Bit  Access 		   				*/
	 unsigned char 	AIER0:1;  /* Address match interrupt0 enable bit*/
	 unsigned char 	AIER1:1;  /* Address match interrupt1 enable bit*/
	 unsigned char 		 :1;  /* Nothing assigned 					*/
	 unsigned char 		 :1;  /* Nothing assigned 					*/
	 unsigned char 		 :1;  /* Nothing assigned  					*/
	 unsigned char 		 :1;  /* Nothing assigned  					*/
	 unsigned char 	 	 :1;  /* Nothing assigned 					*/
	 unsigned char 		 :1;  /* Nothing assigned  					*/
   } BIT;   		      	  /*       						    	*/
	 unsigned char BYTE;      /* Byte Access 						*/
};                            /* Address match interrupt enable register	*/


/*------------------------------------------------------
   Protect register
-----------------------------------------------------*/
union st_prcr {               /* union PRCR    	   					*/
   struct {              	  /* Bit  Access 		   				*/
	 unsigned char 	PRC0:1;   /* Enables writing to system clock control registers 0 & 1 */
	 unsigned char 	PRC1:1;   /* Enables writing to processor mode registers 0 & 1 */
	 unsigned char 	PRC2:1;   /* Enables writing to port P9 direction register & SI/Oi control register(i=3,4)*/
	 unsigned char 		:1;   /* Nothing assigned 					*/
	 unsigned char 		:1;   /* Nothing assigned  					*/
	 unsigned char 		:1;   /* Nothing assigned  					*/
	 unsigned char 	 	:1;   /* Nothing assigned 					*/
	 unsigned char 		:1;   /* Nothing assigned  					*/
   } BIT;   		      	  /*       						    	*/
	 unsigned char BYTE;      /* Byte Access 						*/
};                            /* Protect register					*/

/*------------------------------------------------------
   Watchdog timer control register
-----------------------------------------------------*/
union st_wdc {               /* union WDC    	   					*/
   struct {             	 /* Bit  Access 		   				*/
     unsigned char 	 B0:1; 	 /* High-order bit of watchdog timer	*/
     unsigned char 	 B1:1; 	 /* High-order bit of watchdog timer	*/
     unsigned char 	 B2:1; 	 /* High-order bit of watchdog timer	*/
     unsigned char 	 B3:1;   /* High-order bit of watchdog timer	*/
     unsigned char 	 B4:1; 	 /* High-order bit of watchdog timer	*/
     unsigned char 	 B5:1; 	 /* Reserved bit, must always be 0		*/
     unsigned char 	 B6:1; 	 /* Reserved bit, must always be 0		*/
     unsigned char WDC7:1;   /* Prescaler select bit				*/
   } BIT;   		    	 /*        						    	*/
     unsigned char BYTE;     /*  Byte Access 						*/
};                           /* Watchdog timer control register     */

union st_rmad0 {
   struct{
	unsigned char RMAD0L;     /* Address match interrupt register 0 low  8 bit */
	unsigned char RMAD0M;     /* Address match interrupt register 0 mid  8 bit */
	unsigned char RMAD0H;     /* Address match interrupt register 0 high 8 bit */
	unsigned char NC;         /* non use 									   */
   } BYTE;					  /* Byte access								   */
	unsigned long   DWORD;	  /*	Word Access								   */
};							  /* Address match interrupt register 0 32 bit 	   */


union st_rmad1 {
   struct{
	unsigned char RMAD1L;     /* Address match interrupt register 1 low  8 bit */
	unsigned char RMAD1M;     /* Address match interrupt register 1 mid  8 bit */
	unsigned char RMAD1H;     /* Address match interrupt register 1 high 8 bit */
	unsigned char NC;         /* non use 									   */
   } BYTE;					  /* Byte access								   */
   unsigned long   DWORD;	  /*	Word Access								   */
};							  /* Address match interrupt register 1 32 bit 	   */

union st_sar0 {
   struct{
	unsigned char SAR01;     /* DMA0 source pointer low  8 bit 				 */
	unsigned char SAR0M;     /* DMA0 source pointer mid  8 bit 				 */
	unsigned char SAR0H;     /* DMA0 source pointer high 8 bit 				 */
	unsigned char NC;        /* non use 									 */
   } BYTE;				 	 /* Byte access									 */
   unsigned long   DWORD;	 /*	Word Access									 */
};							 /* DMA0 source pointer 32 bit				 	 */

union st_sar1 {
   struct{
	unsigned char SAR11;     /* DMA1 source pointer low  8 bit 				 */
	unsigned char SAR1M;     /* DMA1 source pointer mid  8 bit 				 */
	unsigned char SAR1H;     /* DMA1 source pointer high 8 bit 				 */
	unsigned char NC;        /* non use 									 */
   } BYTE;					 /* Byte access									 */
   unsigned long   DWORD;	 /* Word Access									 */
};					         /* DMA1 source pointer 32 bit				 	 */

union st_dar0 {				 /* DMA0 destination pointer 32 bit	   			 */
   struct{
	unsigned char DAR01;     /* DMA0 destination pointer low  8 bit  		 */
	unsigned char DAR0M;     /* DMA0 destination pointer mid  8 bit     	 */
	unsigned char DAR0H;     /* DMA0 destination pointer high 8 bit  		 */
	unsigned char NC;        /* non use 							   		 */
   } BYTE;					 /* Byte access						   			 */
    unsigned long DWORD;	 /* Word Access						   			 */
};

union st_dar1 {				  /* DMA1 destination pointer 32 bit			 */
   struct{
	unsigned char DAR11;     /* DMA1 destination pointer low  8 bit			 */
	unsigned char DAR1M;     /* DMA1 destination pointer mid  8 bit			 */
	unsigned char DAR1H;     /* DMA1 destination pointer high 8 bit			 */
	unsigned char NC;        /* non use 									 */
   } BYTE;					 /* Byte access									 */
   unsigned long   DWORD;	 /* Word Access									 */
};

union st_tcr0 {			  	 /* DMA0 transfer counter 16 bit 	 			 */
   struct{
	unsigned char TCR0L;     /* DMA0 transfer counter low  8 bit 			 */
	unsigned char TCR0H;     /* DMA0 transfer counter high 8 bit 			 */
   } BYTE;					 /* Byte access					     			 */
   unsigned short   WORD;	 /* Word Access					     			 */
};

union st_tcr1 {				  /* DMA1 transfer counter 16 bit 	   			 */
   struct{
	unsigned char TCR1L;     /* DMA1 transfer counter low  8 bit   			 */
	unsigned char TCR1H;     /* DMA1 transfer counter high 8 bit   			 */
   } BYTE;					 /* Byte access					       			 */
   unsigned short WORD;	     /* Word Access					       			 */
};


union st_u0tb {				/* UART0 Transmit buffer register 16 bit ; Use "MOV" instruction when writing to this register. */
   struct{
	unsigned char U0TBL;     /* UART0 Transmit buffer register low  8 bit 	 */
	unsigned char U0TBH;     /* UART0 Transmit buffer register high 8 bit 	 */
   } BYTE;					 /* Byte access					   				 */
   unsigned short   WORD;	 /* Word Access					   				 */
};

union st_u1tb {				 /* UART1 Transmit buffer register 16 bit ; Use "MOV" instruction when writing to this register. */
   struct{
	unsigned char U1TBL;     /* UART1 Transmit buffer register low  8 bit    */
	unsigned char U1TBH;     /* UART1 Transmit buffer register high 8 bit    */
   } BYTE;					 /* Byte access					   				 */
   unsigned short   WORD;	 /* Word Access					   				 */
};

union st_u2tb {				 /* UART2 Transmit buffer register 16 bit ; Use "MOV" instruction when writing to this register. */
   struct{
	unsigned char U2TBL;     /* UART2 Transmit buffer register low  8 bit 	 */
	unsigned char U2TBH;     /* UART2 Transmit buffer register high 8 bit  	 */
   } BYTE;				 	 /* Byte access					   				 */
   unsigned short   WORD;	 /* Word Access					   				 */
};

union st_crcd {				 /* CRC data register 16 bit     				 */
   struct{
	unsigned char CRCDL;     /* CRC data register low  8 bit 				 */
	unsigned char CRCDH;     /* CRC data register high 8 bit 				 */
   } BYTE;					 /* Byte access					 				 */
   unsigned short   WORD;    /* Word Access					 				 */
};



union st_ad0 {				 /* A/D register 0 16 bit 						*/
   struct{
	unsigned char AD0L;      /* A/D register 0 low  8 bit 					*/
	unsigned char AD0H;      /* A/D register 0 high 8 bit 					*/
   }BYTE;					 /* Byte access					    			*/
   unsigned short   WORD;	 /* Word Access					    			*/
};

 union st_ad1 {				 /* A/D register 1 16 bit 						*/
   struct{
	unsigned char AD1L;      /* A/D register 1 low  8 bit 					*/
	unsigned char AD1H;      /* A/D register 1 high 8 bit 					*/
   } BYTE;					 /* Byte access					    			*/
   unsigned short   WORD;	 /* Word Access					    			*/
};


union st_ad2 {				 /* A/D register 2 16 bit 						*/
  struct{
	unsigned char AD2L;      /* A/D register 2 low  8 bit 					*/
	unsigned char AD2H;      /* A/D register 2 high 8 bit 					*/
  } BYTE;					 /* Byte access					    			*/
  unsigned short   WORD;	 /* Word Access					    			*/
};

union st_ad3 {			     /* A/D register 3 16 bit 						*/
  struct{
	unsigned char AD3L;      /* A/D register 3 low  8 bit 					*/
	unsigned char AD3H;      /* A/D register 3 high 8 bit 					*/
  } BYTE;					 /* Byte access					    			*/
	unsigned short   WORD;	 /* Word Access					    			*/
};

union st_ad4 {			     /* A/D register 4 16 bit 						*/
  struct{
	unsigned char AD4L;      /* A/D register 4 low  8 bit 					*/
	unsigned char AD4H;      /* A/D register 4 high 8 bit 					*/
  } BYTE;				     /* Byte access					   				*/
  unsigned short   WORD;	 /* Word Access					    			*/
};

union st_ad5 {				 /* A/D register 5 16 bit 						*/
  struct{
	unsigned char AD5L;      /* A/D register 5 low  8 bit 					*/
	unsigned char AD5H;      /* A/D register 5 high 8 bit 					*/
  } BYTE;					 /* Byte access					   				*/
	unsigned short   WORD;	 /* Word Access					   				*/
};

union st_ad6 {				 /* A/D register 6 16 bit 						*/
   struct{
	unsigned char AD6L;      /* A/D register 6 low  8 bit 					*/
	unsigned char AD6H;      /* A/D register 6 high 8 bit 					*/
   } BYTE;					 /* Byte access					    			*/
   unsigned short   WORD;	 /* Word Access					    			*/
 };

union st_ad7 {				 /* A/D register 7 16 bit 						*/
   struct{
	unsigned char AD7L;      /* A/D register 7 low  8 bit 					*/
	unsigned char AD7H;      /* A/D register 7 high 8 bit 					*/
   } BYTE;					 /* Byte access					    			*/
   unsigned short   WORD;	 /* Word Access					    			*/
 };


union st_dm0con {		       /* DMA0 control register union 				*/
   struct{
     unsigned char    DMBIT:1; /* Transfer unit bit select bit 				*/
     unsigned char    DMASL:1; /* Repeat transfer mode select bit 			*/
     unsigned char    DMAS :1; /* DMA request bit 							*/
     unsigned char    DMAE :1; /* DMA enable bit 							*/
     unsigned char    DSD  :1; /* Source address direction select bit   	*/
     unsigned char    DAD  :1; /* Destination address direction select bit  */
     unsigned char    	   :1;
     unsigned char         :1;
   }BIT;
    unsigned char  BYTE;
};

union st_dm1con {		/*    DMA1 control register union */
    struct{
     unsigned char    DMBIT:1; /* Transfer unit bit select bit 				*/
     unsigned char    DMASL:1; /* Repeat transfer mode select bit 			*/
     unsigned char    DMAS :1; /* DMA request bit 							*/
     unsigned char    DMAE :1; /* DMA enable bit 							*/
     unsigned char    DSD  :1; /* Source address direction select bit  		*/
     unsigned char    DAD  :1; /* Destination address direction select bit  */
     unsigned char    	   :1;
     unsigned char         :1;
    }BIT;
    unsigned char  BYTE;
};

union st_dm0sl {			  /*    DMAi request cause select registers			*/
    struct{
     unsigned char    DSEL0:1;/* DMA request cause select bit 				*/
     unsigned char    DSEL1:1;/* DMA request cause select bit 				*/
     unsigned char    DSEL2:1;/* DMA request cause select bit 				*/
     unsigned char    DSEL3:1;/* DMA request cause select bit 				*/
     unsigned char         :1;
     unsigned char         :1;
     unsigned char    DMS  :1;/* DMA request cause expansion bit 			*/
     unsigned char    DSR  :1;/* Software DMA request bit 					*/
    }BIT;
     unsigned char  BYTE;
};


union st_dm1sl {			   /*    DMAi request cause select registers	*/
    struct{
     unsigned char    DSEL0:1; /* DMA request cause select bit 				*/
     unsigned char    DSEL1:1; /* DMA request cause select bit 				*/
     unsigned char    DSEL2:1; /* DMA request cause select bit 				*/
     unsigned char    DSEL3:1; /* DMA request cause select bit 				*/
     unsigned char     	   :1;
     unsigned char    	   :1;
     unsigned char    DMS  :1; /* DMA request cause expansion bit 			*/
     unsigned char    DSR  :1; /* Software DMA request bit 					*/
    }BIT;
    unsigned char  BYTE;
};


union st_icr {		     	   /*    interrupt control registers	  */
    struct{
     unsigned char    ILVL0:1; /* Interrupt priority level select bit */
     unsigned char    ILVL1:1; /* Interrupt priority level select bit */
     unsigned char    ILVL2:1; /* Interrupt priority level select bit */
     unsigned char    IR   :1; /* Interrupt request bit 			  */
     unsigned char    POL  :1; /* Polarity select bit 				  */
     unsigned char    LVS  :1;
     unsigned char    	   :1; /* Nothing assigned					  */
     unsigned char         :1; /* Nothing assigned					  */
    }BIT;
    unsigned char    BYTE;
};



union st_tbsr {              /* union tbsr    	   					*/
	struct {                 /* Bit  Access 		   				*/
	 unsigned char 		:1;  /* Nothing Assigned					*/
	 unsigned char 		:1;  /* Nothing Assigned					*/
	 unsigned char 		:1;  /* Nothing Assigned					*/
	 unsigned char 		:1;  /* Nothing Assigned					*/
	 unsigned char 		:1;  /* Nothing Assigned					*/
	 unsigned char 	TB3S:1;  /* Timer B3 count start flag 			*/
	 unsigned char 	TB4S:1;  /* Timer B4 count start flag 			*/
	 unsigned char 	TB5S:1;  /* Timer B5 count start flag 			*/
	} BIT;   		         /*        						    	*/
	unsigned  char BYTE;     /*  Byte Access 						*/
};                           /* Timer B3,4,5 Count start flag       */

/*------------------------------------------------------
   Three-phase PWM control regester 0
------------------------------------------------------*/
union st_invc0 {            /* union invc0 	   						*/
	struct {            	/* Bit  Access 		   					*/
	 unsigned char 	INV00:1;/* Effective interrupt output polarity select bit */
	 unsigned char 	INV01:1;/* Effective interrupt output specification bit */
	 unsigned char 	INV02:1;/* Mode select bit 						*/
	 unsigned char 	INV03:1;/* Output control bit 					*/
	 unsigned char 	INV04:1;/* Positive and negative phases concurrent L output disable function enable bit */
	 unsigned char 	INV05:1;/* Positive and negative phases concurrent L output detect flag */
	 unsigned char 	INV06:1;/* Modulation mode select bit 			*/
	 unsigned char 	INV07:1;/* Software trigger bit 				*/
	} BIT;   		     	/*        						    	*/
	 unsigned char BYTE;    /*  Byte Access 						*/
};                          /* 						            	*/

/*------------------------------------------------------
    Three-phase PWM control regester 1
------------------------------------------------------*/
union st_invc1 {            /* union invc1 	   							*/
	struct {            	/* Bit  Access 		   						*/
	 unsigned char 	INV00:1;/* Timer Ai start trigger signal select bit */
	 unsigned char 	INV11:1;/* Timer A1-1,A2-1,A4-1 control bit 		*/
	 unsigned char 	INV12:1;/* Short circuit timer count source select bit*/
	 unsigned char 		 :1;/* Nothing Assigned							*/
	 unsigned char 		 :1;/* Reserved bit (always 0)					*/
	 unsigned char 		 :1;/* Nothing Assigned							*/
	 unsigned char 	 	 :1;/* Nothing Assigned							*/
	 unsigned char 		 :1;/* Nothing Assigned							*/
	} BIT;   		     	/*        						    		*/
	unsigned char BYTE;     /*  Byte Access 							*/
};                          /* 						            		*/

/*------------------------------------------------------
    Three-phase output buffer register 0
------------------------------------------------------*/
union st_idb0 {            /* union idb0 	   						*/
	struct {           	   /* Bit  Access 		   					*/
	 unsigned char 	DU0 :1;/* U  phase output buffer 0 				*/
	 unsigned char 	DUB0:1;/* U~ phase output buffer 0 				*/
	 unsigned char 	DV0 :1;/* V  phase output buffer 0 				*/
	 unsigned char 	DVB0:1;/* V~ phase output buffer 0 				*/
	 unsigned char 	DW0 :1;/* W  phase output buffer 0 				*/
	 unsigned char 	DWB0:1;/* W~ phase output buffer 0 				*/
	 unsigned char 		:1;/* Nothing Assigned						*/
	 unsigned char 		:1;/* Nothing Assigned						*/
	} BIT;   		   	   /*        						    	*/
	unsigned char BYTE;    /*  Byte Access 							*/
};                         /* 						            	*/

/*------------------------------------------------------
    Three-phase output buffer register 1
------------------------------------------------------*/
union st_idb1 {            /* union idb1 	   						*/
	struct {	           /* Bit  Access 		   					*/
	 unsigned char 	DU1 :1;/* U  phase output buffer 1				*/
	 unsigned char	DUB1:1;/* U~ phase output buffer 1 				*/
	 unsigned char	DV1 :1;/* V  phase output buffer 1 				*/
	 unsigned char	DVB1:1;/* V~ phase output buffer 1 				*/
	 unsigned char	DW1 :1;/* W  phase output buffer 1 				*/
	 unsigned char	DWB1:1;/* W~ phase output buffer 1 				*/
	 unsigned char		:1;/* Nothing Assigned						*/
	 unsigned char		:1;/* Nothing Assigned						*/
	} BIT;  	 		   /*        						    	*/
	unsigned char BYTE;    /*  Byte Access 							*/
};                         /* 						            	*/


/*------------------------------------------------------
   Timer mode registers
------------------------------------------------------*/
union st_tmr {               /* union tmr    	   					*/
	struct {             	 /* Bit  Access 		   				*/
	 unsigned char 	TMOD0:1; /* Operation mode select bit 			*/
	 unsigned char	TMOD1:1; /* Operation mode select bit 			*/
	 unsigned char	MR0	 :1; /* Pulse output function select bit	*/
	 unsigned char	MR1  :1; /* External trigger select bit			*/
	 unsigned char	MR2  :1; /* Trigger select bit					*/
	 unsigned char	MR3  :1; /* Must always be "0" in one-shot timer*/
	 unsigned char	TCK0 :1; /* Count source select bit 			*/
	 unsigned char	TCK1 :1; /* Count source select bit 			*/
	} BIT;   		     	 /*       						    	*/
	unsigned char BYTE;      /*  Byte Access 						*/
};


/*------------------------------------------------------
Interrupt request cause select register
-----------------------------------------------------*/
union st_ifsr {              /* union IFSR    	   						*/
	struct {             	 /* Bit  Access 		   					*/
	 unsigned char 	IFSR0:1; /* INT0~ interrupt polarity switching bit  */
	 unsigned char	IFSR1:1; /* INT1~ interrupt polarity switching bit  */
	 unsigned char	IFSR2:1; /* INT2~ interrupt polarity switching bit  */
	 unsigned char	IFSR3:1; /* INT3~ interrupt polarity switching bit  */
	 unsigned char	IFSR4:1; /* INT4~ interrupt polarity switching bit  */
	 unsigned char	IFSR5:1; /* INT5~ interrupt polarity switching bit  */
	 unsigned char	IFSR6:1; /* Interrupt request cause select bit 		*/
	 unsigned char	IFSR7:1; /* Interrupt request cause select bit 		*/
	} BIT;   		     	 /*        						    		*/
	unsigned char BYTE;      /*  Byte Access 							*/
};                           /* 						            	*/

/*------------------------------------------------------
   SI/O3 control registers
------------------------------------------------------*/
union st_s3c {              /* union S3C    	   						*/
	struct {            	/* Bit  Access 		   						*/
	 unsigned char 	SM30:1; /* Internal synchronous clock select bit 	*/
	 unsigned char	SM31:1; /* Internal synchronous clock select bit 	*/
	 unsigned char	SM32:1; /* Sout3 output disable bit 				*/
	 unsigned char	SM33:1; /* SI/O3 port select bit 					*/
	 unsigned char		:1;
	 unsigned char	SM35:1; /* Transfer direction select bit 			*/
	 unsigned char	SM36:1; /* Synchronous clock select bit 			*/
	 unsigned char	SM37:1; /* Sout3 initial value set bit 				*/
	} BIT;   		   		/*        						    		*/
	unsigned char BYTE;     /*  Byte Access 							*/
};                          /* 						            		*/

/*------------------------------------------------------
   SI/O4 control registers
------------------------------------------------------*/
union st_s4c {              /* union S4C    	   						*/
	struct {   		        /* Bit  Access 		   						*/
	 unsigned char 	SM40:1; /* Internal synchronous clock select bit 	*/
	 unsigned char	SM41:1; /* Internal synchronous clock select bit 	*/
	 unsigned char	SM42:1; /* Sout4 output disable bit					*/
	 unsigned char	SM43:1; /* SI/O4 port select bit 					*/
	 unsigned char		:1; /* Nothing assigned							*/
	 unsigned char	SM45:1; /* Transfer direction select bit 			*/
	 unsigned char	SM46:1; /* Synchronous clock select bit 			*/
	 unsigned char	SM47:1; /* Sout4 initial value set bit 				*/
	} BIT;   		    	/*        						    		*/
	unsigned char BYTE;     /*  Byte Access 							*/
};

/*------------------------------------------------------
    UART2 special mode register 3
------------------------------------------------------*/
union st_u2smr3 {           /* union U2SMR3    	   					*/
	struct {            	/* Bit  Access 		   					*/
	 unsigned char 		:1; /* 										*/
	 unsigned char		:1; /* 										*/
	 unsigned char		:1; /* 										*/
	 unsigned char		:1; /* 										*/
	 unsigned char		:1; /* 										*/
	 unsigned char	 DL0:1; /* SDA digital delay setup bit 			*/
	 unsigned char	 DL1:1; /* SDA digital delay setup bit 			*/
	 unsigned char	 DL2:1; /* SDA digital delay setup bit 			*/
	} BIT;   		    	/*        						    	*/
	unsigned char BYTE;     /*  Byte Access 						*/
};                          /* 						            	*/

/*------------------------------------------------------
    UART2 special mode register 2
------------------------------------------------------*/
union st_u2smr2 {            /* union U2SMR2    	   					*/
	struct {	             /* Bit  Access 		   					*/
	 unsigned char 	IICM2:1; /* IIC mode selection bit 2				*/
	 unsigned char	CSC  :1; /* Clock-synchronous bit 					*/
	 unsigned char	SWC  :1; /* SCL wait output bit 					*/
	 unsigned char	ALS  :1; /* SDA output stop bit 					*/
	 unsigned char	STAC :1; /* UART2 initialization bit 				*/
	 unsigned char	SWC2 :1; /* SCL wait output bit 2 					*/
	 unsigned char	SDHI :1; /* SDA output disable bit					*/
	 unsigned char	SHTC :1; /* Start/stop condition control bit 		*/
	} BIT;   			     /*        						    		*/
	unsigned char BYTE;      /*  Byte Access 							*/
};                           /* 						            	*/

/*------------------------------------------------------
    UART2 special mode register
------------------------------------------------------*/
union st_u2smr {             /* union U2SMR    	   							*/
	struct {             	 /* Bit  Access 		  	 					*/
	 unsigned char 	IICM :1; /* IIC mode selection bit						*/
	 unsigned char	ABC  :1; /* Arbitration lost detecting flag control bit */
	 unsigned char	BBS  :1; /* Bus busy flag 								*/
	 unsigned char	LSYN :1; /* SCLL sync output enable bit 				*/
	 unsigned char	ABSCS:1; /* Bus collision detect sampling clock select bit */
	 unsigned char	ACSE :1; /* Auto clear function select bit of transmit enable bit */
	 unsigned char	SSS  :1; /* Transmit start condition select bit			*/
	 unsigned char	SDDS :1; /* SDA digital delay select bit 				*/
	} BIT;   		     	 /*        						 		   		*/
	unsigned char BYTE;      /*  Byte Access 								*/
};                           /* 						     		       	*/


/*------------------------------------------------------
    UARTi transmit/receive mode register
------------------------------------------------------*/
union st_u2mr {              	  /* union U2MR	   							*/
	struct {             	      /* Bit  Access	  	 					*/
	 unsigned char 	SMD0_U2MR :1; /* Serial I/O mode select bit 			*/
	 unsigned char	SMD1_U2MR :1; /* Serial I/O mode select bit 			*/
	 unsigned char	SMD2_U2MR :1; /* Serial I/O mode select bit 			*/
	 unsigned char	CKDIR_U2MR:1; /* Internal/external clock select bit		*/
	 unsigned char	STPS_U2MR :1; /* Stop bit length select bit 			*/
	 unsigned char	PRY_U2MR  :1; /* Odd/even parity select bit 			*/
	 unsigned char	PRYE_U2MR :1; /* Parity enable bit 						*/
	 unsigned char	IOPOL_U2MR:1; /* TxD RxD I/O polarity reverse bit 		*/
	} BIT;   		     		  /*        						 		   		*/
	unsigned char BYTE;           /*  Byte Access							*/
};

/*------------------------------------------------------
     u1mr
------------------------------------------------------*/
union st_u1mr {              	  /* union U1MR	   							*/
	struct {             	 	  /* Bit  Access	  	 					*/
	 unsigned char 	SMD0_U1MR :1; /* Serial I/O mode select bit 			*/
	 unsigned char	SMD1_U1MR :1; /* Serial I/O mode select bit 			*/
	 unsigned char	SMD2_U1MR :1; /* Serial I/O mode select bit 			*/
	 unsigned char	CKDIR_U1MR:1; /* Internal/external clock select bit 	*/
	 unsigned char	STPS_U1MR :1; /* Stop bit length select bit 			*/
	 unsigned char	PRY_U1MR  :1; /* Odd/even parity select bit 			*/
	 unsigned char	PRYE_U1MR :1; /* Parity enable bit 						*/
	 unsigned char	SLEP_U1MR :1; /* Sleep select bit 						*/
	} BIT;   		     	  	  /*   						 		   		*/
	unsigned char BYTE;           /* Byte Access 							*/
};


/*------------------------------------------------------
    u0mr
------------------------------------------------------*/
union st_u0mr {              	  /* union U0MR	   							*/
	struct {             	      /* Bit  Access	  	 					*/
	 unsigned char 	SMD0_U0MR :1; /* Serial I/O mode select bit 			*/
	 unsigned char 	SMD1_U0MR :1; /* Serial I/O mode select bit 			*/
	 unsigned char 	SMD2_U0MR :1; /* Serial I/O mode select bit 			*/
	 unsigned char 	CKDIR_U0MR:1; /* Internal/external clock select bit 	*/
	 unsigned char 	STPS_U0MR :1; /* Stop bit length select bit 			*/
	 unsigned char 	PRY_U0MR  :1; /* Odd/even parity select bit 			*/
	 unsigned char 	PRYE_U0MR :1; /* Parity enable bit 						*/
	 unsigned char 	SLEP_U0MR :1; /* Sleep select bit 						*/
	} BIT;   		     	  	  /*   						 		   		*/
	unsigned char BYTE;           /* Byte Access 							*/
};

/*------------------------------------------------------
    UARTi transmit/receive control register 0
------------------------------------------------------*/
union st_u0c0 {              	/* union U0C0	   							*/
	struct {             		/* Bit  Access	  	 						*/
	 unsigned char 	CLK0 :1; 	/* BRG count source select bit 				*/
	 unsigned char	CLK1 :1; 	/* BRG count source select bit 				*/
	 unsigned char	CRS  :1; 	/* CTS~/RTS~ function select bit 			*/
	 unsigned char	TXEPT:1; 	/* Transmit register empty flag 			*/
	 unsigned char	CRD  :1; 	/* CTS~/RTS~ disable bit 					*/
	 unsigned char	NCH  :1; 	/* Data output select bit 					*/
	 unsigned char	CKPOL:1; 	/* CLK polarity select bit 					*/
	 unsigned char	UFORM:1; 	/* Transfer format select bit 				*/
	} BIT;   		     		/*   						 		   		*/
	unsigned char BYTE;         /* Byte Access 								*/
}; 								/*UARTi transmit/receive control register 0 */

/*------------------------------------------------------
     u1c0
------------------------------------------------------*/
union st_u1c0 {              	/* union UCR	   							*/
	struct {             		/* Bit  Access	  	 						*/
	 unsigned char 	CLK0 :1; 	/* BRG count source select bit 				*/
	 unsigned char	CLK1 :1; 	/* BRG count source select bit 				*/
	 unsigned char	CRS  :1; 	/* CTS~/RTS~ function select bit 			*/
	 unsigned char	TXEPT:1; 	/* Transmit register empty flag 			*/
	 unsigned char	CRD  :1; 	/* CTS~/RTS~ disable bit 					*/
	 unsigned char	NCH  :1; 	/* Data output select bit 					*/
	 unsigned char	CKPOL:1; 	/* CLK polarity select bit 					*/
	 unsigned char	UFORM:1; 	/* Transfer format select bit 				*/
	} BIT;   		     		/*   						 		   		*/
	unsigned char BYTE;         /* Byte Access 								*/
}; 								/*UARTi transmit/receive control register 0 */


/*------------------------------------------------------
     u2c0
------------------------------------------------------*/
union st_u2c0 {              	/* union U2C0	   							*/
	struct {             		/* Bit  Access	  	 						*/
	 unsigned char 	CLK0 :1; 	/* BRG count source select bit 				*/
	 unsigned char	CLK1 :1; 	/* BRG count source select bit 				*/
	 unsigned char	CRS  :1; 	/* CTS~/RTS~ function select bit 			*/
	 unsigned char	TXEPT:1; 	/* Transmit register empty flag 			*/
	 unsigned char	CRD  :1; 	/* CTS~/RTS~ disable bit 					*/
	 unsigned char		 :1; 	/* Nothing Assigned							*/
	 unsigned char	CKPOL:1; 	/* CLK polarity select bit 					*/
	 unsigned char	UFORM:1; 	/* Transfer format select bit 				*/
	} BIT;   		     		/*   						 		   		*/
	unsigned char BYTE;         /* Byte Access 								*/
}; 								/* UART2 transmit/receive control register 0*/

/*------------------------------------------------------
    UART2 transmit/receive control register 1
-----------------------------------------------------*/
union st_u2c1 {              	/* union U2C1	   							*/
	struct {             		/* Bit  Access	  	 						*/
	 unsigned char 	TE_U2C1:1; 	/* Transmit enable bit 						*/
	 unsigned char	TI_U2C1:1; 	/* Transmit buffer empty flag 				*/
	 unsigned char	RE_U2C1:1; 	/* Receive enable bit 						*/
	 unsigned char	RI_U2C1:1; 	/* Receive complete flag 					*/
	 unsigned char	U2IRS  :1; 	/* UART2 transmit interrupt cause select bit*/
	 unsigned char	U2RRM  :1; 	/* UART2 continuous receive mode enable bit */
	 unsigned char	U2LCH  :1; 	/* Data logic select bit 					*/
	 unsigned char	U2ERE  :1; 	/* Error signal output enable bit 			*/
	} BIT;   		     		/*   						 		   		*/
	unsigned char BYTE;         /* Byte Access 								*/
}; 								/*UART2 transmit/receive control register 1 */

/*------------------------------------------------------
    UARTi receive buffer registers
------------------------------------------------------*/
union st_u2rb {     				/* UART2 receive buffer register */
    struct {						/*  Bit  Access */
	 unsigned char 			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char   		:1;		/*  Receive data       				*/
	 unsigned char   		:1;		/* Nothing assigned	   				*/
	 unsigned char   		:1;		/* Nothing assigned	   				*/
 	 unsigned char  ABT_U2RB:1;     /* Arbitration lost detecting flag  */
 	 unsigned char  OER_U2RB:1;     /* Overrun error flag 				*/
 	 unsigned char  FER_U2RB:1;     /* Framing error flag				*/
 	 unsigned char  PER_U2RB:1;     /* Parity error flag 				*/
 	 unsigned char  SUM_U2RB:1;     /* Error sum flag 					*/
    }BIT;
    struct{
 	 unsigned char    U2RBL;        /* Low  8 bit						*/
 	 unsigned char    U2RBH;        /* High 8 bit 						*/
    }BYTE;
    unsigned short  WORD;
};

/*------------------------------------------------------
     u0rb
------------------------------------------------------*/
union st_u0rb {						/*  UART0 receive buffer register 	*/
    struct {						/*  Bit  Access 				 	*/
	 unsigned char 			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char  		:1;		/*  Receive data       				*/
	 unsigned char  		:1;		/* Nothing assigned	   				*/
	 unsigned char  		:1;		/* Nothing assigned	   				*/
 	 unsigned char          :1;     /* 									*/
 	 unsigned char  OER_U0RB:1;     /* Overrun error flag 				*/
 	 unsigned char  FER_U0RB:1;     /* Framing error flag				*/
 	 unsigned char  PER_U0RB:1;     /* Parity error flag 				*/
 	 unsigned char  SUM_U0RB:1;     /* Error sum flag 					*/
    }BIT;
    struct{
     unsigned char U0RBL;        	/* Low  8 bit 						*/
     unsigned char U0RBH;           /* High 8 bit 						*/
    }BYTE;
    unsigned short  WORD;
};

/*------------------------------------------------------
     u1rb
------------------------------------------------------*/
union st_u1rb {						/* UART1 receive buffer register */
    struct {						/*  Bit  Access */
	 unsigned char 			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char			:1;		/*  Receive data       				*/
	 unsigned char  		:1;		/*  Receive data       				*/
	 unsigned char  		:1;		/* Nothing assigned	   				*/
	 unsigned char  		:1;		/* Nothing assigned	   				*/
 	 unsigned char          :1;     /* 									*/
 	 unsigned char  OER_U1RB:1;     /* Overrun error flag 				*/
 	 unsigned char  FER_U1RB:1;     /* Framing error flag				*/
 	 unsigned char  PER_U1RB:1;     /* Parity error flag 				*/
 	 unsigned char  SUM_U1RB:1;     /* Error sum flag 					*/
    }BIT;
    struct{
     unsigned char U1RBL;           /* Low  8 bit 						*/
     unsigned char U1RBH;           /* High 8 bit 						*/
    }BYTE;
    unsigned short  WORD;
};

/*------------------------------------------------------
    Count start flag
------------------------------------------------------*/
union st_tabsr {              	/* union TABSR	   							*/
	struct {             		/* Bit  Access	  	 						*/
     unsigned char	TA0S:1; 	/* Timer A0 count start flag 				*/
     unsigned char	TA1S:1; 	/* Timer A1 count start flag 				*/
     unsigned char	TA2S:1; 	/* Timer A2 count start flag 				*/
     unsigned char	TA3S:1; 	/* Timer A3 count start flag 				*/
     unsigned char	TA4S:1; 	/* Timer A4 count start flag 				*/
     unsigned char	TB0S:1; 	/* Timer B0 count start flag 				*/
     unsigned char	TB1S:1; 	/* Timer B1 count start flag 				*/
     unsigned char	TB2S:1; 	/* Timer B2 count start flag 				*/
	} BIT;   		     		/*   						 		   		*/
	unsigned char BYTE;         /* Byte Access 								*/
}; 								/*UART2 transmit/receive control register 1 */


/*------------------------------------------------------
    Clock prescaler reset flag
------------------------------------------------------*/
union st_cpsrf {              /* union CPSRF    	   						*/
              	struct {      /* Bit  Access		   						*/
     unsigned char 		:1;   /* 											*/
     unsigned char 		:1;   /* 											*/
     unsigned char 		:1;   /* 											*/
     unsigned char 		:1;   /* 											*/
     unsigned char 		:1;   /* 				 							*/
     unsigned char 		:1;   /* 				 							*/
     unsigned char 	 	:1;   /* 											*/
     unsigned char 	CPSR:1;   /* Clock prescaler reset flag 				*/
                } BIT;   	  /*       						    			*/
     unsigned char BYTE;      /* Byte Access 								*/
};                            /* Watchdog timer start register				*/

/*------------------------------------------------------
    One-shot start flag
------------------------------------------------------*/
union st_onsf {              	/* union ONSF	   							*/
	struct {             		/* Bit  Access	  	 						*/
	 unsigned char 	 TA0OS:1; 	/* Timer A0 one-shot start flag 			*/
	 unsigned char 	 TA1OS:1; 	/* Timer A1 one-shot start flag 			*/
	 unsigned char	 TA2OS:1; 	/* Timer A2 one-shot start flag 			*/
	 unsigned char	 TA3OS:1; 	/* Timer A3 one-shot start flag 			*/
	 unsigned char	 TA4OS:1; 	/* Timer A4 one-shot start flag 			*/
	 unsigned char		  :1; 	/* 											*/
	 unsigned char	TA0TGL:1; 	/* Timer A0 event/trigger select bit 		*/
	 unsigned char	TA0TGH:1; 	/* Timer A0 event/trigger select bit 		*/
	} BIT;   		     		/*   						 		   		*/
	unsigned char BYTE;         /* Byte Access 								*/
}; 								/*UART2 transmit/receive control register 1 */

/*------------------------------------------------------
    Trigger select register
------------------------------------------------------*/
union st_trgsr {              	/* union TRGSR	   							*/
		struct {             	/* Bit  Access	  	 						*/
	 unsigned char 	TA1TGL:1; 	/* Timer A1 event/trigger select bit 		*/
	 unsigned char	TA1TGH:1; 	/* Timer A1 event/trigger select bit 		*/
	 unsigned char	TA2TGL:1; 	/* Timer A2 event/trigger select bit 		*/
	 unsigned char	TA2TGH:1; 	/* Timer A2 event/trigger select bit 		*/
	 unsigned char	TA3TGL:1; 	/* Timer A3 event/trigger select bit 		*/
	 unsigned char	TA3TGH:1; 	/* Timer A3 event/trigger select bit 		*/
	 unsigned char	TA4TGL:1; 	/* Timer A4 event/trigger select bit 		*/
	 unsigned char	TA4TGH:1; 	/* Timer A4 event/trigger select bit 		*/
	} BIT;   			     	/*   						 		   		*/
	unsigned char BYTE;           	/* Byte Access 								*/
}; 								/*UART2 transmit/receive control register 1 */

#if 0
/*------------------------------------------------------
    Timer mode registers
------------------------------------------------------*/
union st_tmr {
    struct{
     unsigned char  TMOD0:1;            /* Operation mode select bit */
     unsigned char  TMOD1:1;            /* Operation mode select bit */
     unsigned char  MR0  :1;
     unsigned char  MR1  :1;
     unsigned char  MR2  :1;
     unsigned char  MR3  :1;
     unsigned char  TCK0 :1;             /* Count source select bit */
     unsigned char  TCK1 :1;             /* Count source select bit */
    }BIT;
    unsigned char    BYTE;
};
#endif
/*------------------------------------------------------
    UARTi transmit/receive control register 1
------------------------------------------------------*/
union st_u0c1 {             /* union U0C1		   							*/
	struct {            	/* Bit  Access	  	 							*/
	 unsigned char 	TE:1; 	/* Transmit enable bit 							*/
	 unsigned char	TI:1; 	/* Transmit buffer empty flag 					*/
	 unsigned char	RE:1; 	/* Receive enable bit 							*/
	 unsigned char	RI:1; 	/* Receive complete flag 						*/
	 unsigned char	  :1; 	/* Nothing Assigned						 		*/
	 unsigned char	  :1; 	/* Nothing Assigned						 		*/
	 unsigned char	  :1; 	/* Nothing Assigned						 		*/
	 unsigned char	  :1; 	/* Nothing Assigned						 		*/
	} BIT;   		    	/*   						 		   			*/
	unsigned char BYTE;     /* Byte Access 									*/
}; 							/*UART0 transmit/receive control register 1 	*/

union st_u1c1 {             /* union U0C1	   								*/
	struct {            	/* Bit  Access	  	 							*/
	 unsigned char 	TE:1; 	/* Transmit enable bit 							*/
	 unsigned char	TI:1; 	/* Transmit buffer empty flag 					*/
	 unsigned char	RE:1; 	/* Receive enable bit 							*/
	 unsigned char	RI:1; 	/* Receive complete flag 						*/
	 unsigned char	  :1; 	/*  											*/
	 unsigned char	  :1; 	/*  											*/
	 unsigned char	  :1; 	/*  											*/
	 unsigned char	  :1; 	/*  											*/
	} BIT;   		     	/*   						 		   		    */
	 unsigned char BYTE;    /* Byte Access 								    */
}; 							/*UART1 transmit/receive control register 1     */

/*------------------------------------------------------
    UART transmit/receive control register 2
------------------------------------------------------*/
union st_ucon {              	/* union UCON   							*/
	struct {             		/* Bit  Access	  	 						*/
	 unsigned char 	U0IRS :1; 	/* UART0 transmit interrupt cause select bit*/
	 unsigned char	U1IRS :1; 	/* UART1 transmit interrupt cause select bit*/
	 unsigned char	U0RRM :1; 	/* UART0 continuous receive mode enable bit */
	 unsigned char	U1RRM :1; 	/* UART1 continuous receive mode enable bit */
	 unsigned char	CLKMD0:1; 	/* CLK/CLKS select bit 0					*/
	 unsigned char	CLKMD1:1; 	/* CLK/CLKS select bit 1					*/
	 unsigned char		  :1; 	/* 											*/
	 unsigned char	      :1; 	/* 											*/
	} BIT;   		     		/*   						 		   		*/
	unsigned char BYTE;        /* Byte Access 								*/
}; 								/*UART transmit/receive control register 2  */



/*------------------------------------------------------
    Flash memory control register 1
------------------------------------------------------*/
union st_fmr1 {                  /* union FMR1    	   						 */
   struct {          			 /* Bit  Access 		   			 		 */
     unsigned char 	     :1; 	 /* 										 */
     unsigned char 	     :1; 	 /* 										 */
     unsigned char 	     :1; 	 /* 										 */
     unsigned char 	FMR13:1;  	 /* Flash memory power supply-OFF bit*/
     unsigned char 	     :1; 	 /* 										 */
     unsigned char 	     :1; 	 /* 										 */
     unsigned char 	     :1; 	 /* 										 */
     unsigned char 	     :1;     /* 										 */
   } BIT;   		  			 /*      						    	     */
   unsigned char BYTE;        	 /*  Byte Access 							 */
};                               /* Flash memory control register      	     */

/*------------------------------------------------------
    Flash memory control register 0
------------------------------------------------------*/
union st_fmr0 {              /* union FMR0    	   						 	*/
    struct {                 /* Bit  Access 		   			 				 */
     unsigned char 	FMR00:1; /* RY/BY~ status bit 							*/
     unsigned char 	FMR01:1; /* CPU rewrite mode select bit 				*/
     unsigned char 	FMR02:1; /* Lock bit disable bit 						*/
     unsigned char 	FMR03:1; /* Flash memory reset bit 						*/
     unsigned char 		 :1;
     unsigned char 	FMR05:1; /* User ROM area select bit 					*/
     unsigned char 	     :1;
     unsigned char 	     :1; /*												*/
   } BIT;   		  		 /*      						    			*/
   unsigned char BYTE;       /*  Byte Access 								*/
};                           /* Flash memory control register      			*/

/*------------------------------------------------------
    A/D control register 2
------------------------------------------------------*/
union st_adcon2 {				/* union ADCON2						*/
    struct {					/*  Bit  Access 					*/
	 unsigned char     SMP:1;	/* A/D conversion method select bit */
	 unsigned char    	:1;		/* Reserved bit (Always set to 0 )  */
	 unsigned char    	:1;		/* Reserved bit (Always set to 0 )  */
	 unsigned char    	:1;		/* Reserved bit (Always set to 0 )  */
	 unsigned char    	:1;		/* Nothing Assigned.		        */
	 unsigned char    	:1;		/* Nothing Assigned.		        */
	 unsigned char    	:1;		/* Nothing Assigned.		        */
	 unsigned char    	:1;		/* Nothing Assigned.		        */
	} BIT;
    unsigned char   BYTE;		/*  Byte Access 					*/
};


/*------------------------------------------------------
    A/D control register 0
------------------------------------------------------*/
union st_adcon0 {              	/* union ADCON0	   							*/
	struct {             		/* Bit  Access	  	 						*/
	 unsigned char 	CH0 :1; 	/* Analog input pin select bit 				*/
	 unsigned char	CH1 :1; 	/* Analog input pin select bit 				*/
	 unsigned char	CH2 :1; 	/* Analog input pin select bit 				*/
	 unsigned char	MD0 :1; 	/* A/D operation mode select bit 0 			*/
	 unsigned char	MD1 :1; 	/* A/D operation mode select bit 0 			*/
	 unsigned char	TRG :1; 	/* Trigger select bit 						*/
	 unsigned char	ADST:1; 	/* A/D conversion start flag 				*/
	 unsigned char	CKS0:1; 	/* Frequency select bit 0 					*/
	} BIT;   		     		/*   						 		   		*/
	unsigned char BYTE;         /* Byte Access 								*/
}; 								/*A/D control register 0					*/


/*------------------------------------------------------
   A/D control register 1
------------------------------------------------------*/
union st_adcon1 {              	/* union ADCON1	   							*/
	struct {             		/* Bit  Access	  	 						*/
	 unsigned char 	SCAN0:1; 	/* A/D sweep pin select bit 				*/
	 unsigned char	SCAN1:1; 	/* A/D sweep pin select bit 				*/
	 unsigned char	MD2  :1; 	/* A/D operation mode select bit 1 			*/
	 unsigned char	BITS :1; 	/* 8/10-bit mode select bit 				*/
	 unsigned char	CKS1 :1; 	/* Frequency select bit 1 					*/
	 unsigned char	VCUT :1; 	/* Vref connect bit							*/
	 unsigned char	OPA0 :1; 	/* External op-amp connection mode bit 		*/
	 unsigned char	OPA1 :1; 	/* External op-amp connection mode bit 		*/
	} BIT;   		     		/*   						 		   		*/
	unsigned char BYTE;         /* Byte Access 								*/
}; 								/*A-D control register 1					*/


/*------------------------------------------------------
    D/A control register
------------------------------------------------------*/
union st_dacon{               /* union DACON    	   					*/
	struct {              	  /* Bit  Access 		   				*/
	 unsigned char 	DA0E :1;  /* D/A0 output enable bit	 		    */
	 unsigned char	DA1E :1;  /* D/A1 output enable bit 			*/
	 unsigned char		 :1;  /* Nothing Assigned 					*/
	 unsigned char		 :1;  /* Nothing Assigned 					*/
	 unsigned char		 :1;  /* Nothing Assigned 					*/
	 unsigned char		 :1;  /* Nothing Assigned 					*/
	 unsigned char	 	 :1;  /* Nothing Assigned 					*/
	 unsigned char		 :1;  /* Nothing Assigned 					*/
	} BIT;   		      	  /*       						    	*/
	unsigned char BYTE;       /* Byte Access 						*/
};                            /* D/A control register				*/



/*------------------------------------------------------
    Port P0 register
------------------------------------------------------*/
union st_p0 {               /* union P0    	   					*/
	struct {            	/* Bit  Access 		   				*/
	 unsigned char 	P0_0:1; /* Port P00 register 				*/
	 unsigned char	P0_1:1; /* Port P01 register 				*/
	 unsigned char	P0_2:1; /* Port P02 register 				*/
	 unsigned char	P0_3:1; /* Port P03 register 				*/
	 unsigned char	P0_4:1; /* Port P04 register 				*/
	 unsigned char	P0_5:1; /* Port P05 register 				*/
	 unsigned char	P0_6:1; /* Port P06 register 				*/
	 unsigned char	P0_7:1; /* Port P07 register 				*/
	} BIT;   		    	/*        						    */
	unsigned char BYTE;     /*  Byte Access 					*/
};                          /* 						            */

/*------------------------------------------------------
    Port P0 direction register
------------------------------------------------------*/
union st_pd0 {              /* union PD0    	   		   		*/
	struct {            	/* Bit  Access 		   	  	   		*/
	 unsigned char 	PD0_0:1;/* Port P00 direction register 		*/
	 unsigned char	PD0_1:1;/* Port P01 direction register 		*/
	 unsigned char	PD0_2:1;/* Port P02 direction register 		*/
	 unsigned char	PD0_3:1;/* Port P03 direction register 		*/
	 unsigned char	PD0_4:1;/* Port P04 direction register 		*/
	 unsigned char	PD0_5:1;/* Port P05 direction register 		*/
	 unsigned char	PD0_6:1;/* Port P06 direction register 		*/
	 unsigned char	PD0_7:1;/* Port P07 direction register 		*/
	} BIT;   		    	/*        					   		*/
	unsigned char BYTE;     /*  Byte Access 			   		*/
};                          /* 						       		*/

/*------------------------------------------------------
    Port P1 register
------------------------------------------------------*/
union st_p1 {              /* union P1	    	   		    	*/
	struct {           	   /* Bit  Access 		   	  	    	*/
	 unsigned char 	P1_0:1;/* Port P10 register			    	*/
	 unsigned char	P1_1:1;/* Port P11 register 				*/
	 unsigned char	P1_2:1;/* Port P12 register 				*/
	 unsigned char	P1_3:1;/* Port P13 register 				*/
	 unsigned char	P1_4:1;/* Port P14 register 				*/
	 unsigned char	P1_5:1;/* Port P15 register 				*/
	 unsigned char	P1_6:1;/* Port P16 register 				*/
	 unsigned char	P1_7:1;/* Port P17 register 				*/
	} BIT;   		       /*        					    	*/
	unsigned char BYTE;    /*  Byte Access 			        	*/
};                         /* 						        	*/

/*------------------------------------------------------
    Port P1 direction register
------------------------------------------------------*/
union st_pd1 {              /* union PD1    	   		   */
	struct {            	/* Bit  Access 		   	  	   */
	 unsigned char 	PD1_0:1;/* Port P10 direction register */
	 unsigned char	PD1_1:1;/* Port P11 direction register */
	 unsigned char	PD1_2:1;/* Port P12 direction register */
	 unsigned char	PD1_3:1;/* Port P13 direction register */
	 unsigned char	PD1_4:1;/* Port P14 direction register */
	 unsigned char	PD1_5:1;/* Port P15 direction register */
	 unsigned char	PD1_6:1;/* Port P16 direction register */
	 unsigned char	PD1_7:1;/* Port P17 direction register */
	} BIT;   		    	/*        					   */
	unsigned char BYTE;     /*  Byte Access 			   */
};                          /* 						       */

/*------------------------------------------------------
    Port P2 register
------------------------------------------------------*/
union st_p2 {              /* union P2	    	   		    */
	struct {           	   /* Bit  Access 		   	  	    */
	 unsigned char 	P2_0:1;/* Port P20 register			    */
	 unsigned char	P2_1:1;/* Port P21 register 			*/
	 unsigned char	P2_2:1;/* Port P22 register 			*/
	 unsigned char	P2_3:1;/* Port P23 register 			*/
	 unsigned char	P2_4:1;/* Port P24 register 			*/
	 unsigned char	P2_5:1;/* Port P25 register 			*/
	 unsigned char	P2_6:1;/* Port P26 register 			*/
	 unsigned char	P2_7:1;/* Port P27 register 			*/
	} BIT;   		  	   /*        					    */
	unsigned char BYTE;    /*  Byte Access 			       	*/
};                         /* 						        */

/*------------------------------------------------------
    Port P2 direction register
------------------------------------------------------*/
union st_pd2 {              /* union PD2    	   		   */
	struct {           	    /* Bit  Access 		   	  	   */
	 unsigned char 	PD2_0:1;/* Port P20 direction register */
	 unsigned char	PD2_1:1;/* Port P21 direction register */
	 unsigned char	PD2_2:1;/* Port P22 direction register */
	 unsigned char	PD2_3:1;/* Port P23 direction register */
	 unsigned char	PD2_4:1;/* Port P24 direction register */
	 unsigned char	PD2_5:1;/* Port P25 direction register */
	 unsigned char	PD2_6:1;/* Port P26 direction register */
	 unsigned char	PD2_7:1;/* Port P27 direction register */
		} BIT;   		    /*        					   */
	unsigned char BYTE;     /*  Byte Access 			   */
};                          /* 						       */

/*------------------------------------------------------
    Port P3 register
------------------------------------------------------*/
union st_p3 {              /* union P3	    	   		    */
	struct {           	   /* Bit  Access 		   	  	    */
	 unsigned char 	P3_0:1;/* Port P30 register			    */
	 unsigned char	P3_1:1;/* Port P31 register 			*/
	 unsigned char	P3_2:1;/* Port P32 register 			*/
	 unsigned char	P3_3:1;/* Port P33 register 			*/
	 unsigned char	P3_4:1;/* Port P34 register 			*/
	 unsigned char	P3_5:1;/* Port P35 register 			*/
	 unsigned char	P3_6:1;/* Port P36 register 			*/
	 unsigned char	P3_7:1;/* Port P37 register 			*/
	} BIT;   		   	   /*        					    */
	unsigned char BYTE;    /*  Byte Access 			        */
};                         /* 						        */

/*------------------------------------------------------
    Port P3 direction register
------------------------------------------------------*/
union st_pd3 {              /* union PD3    	   		   */
	struct {           	    /* Bit  Access 		   	  	   */
	 unsigned char 	PD3_0:1;/* Port P30 direction register */
	 unsigned char	PD3_1:1;/* Port P31 direction register */
	 unsigned char	PD3_2:1;/* Port P32 direction register */
	 unsigned char	PD3_3:1;/* Port P33 direction register */
	 unsigned char	PD3_4:1;/* Port P34 direction register */
	 unsigned char	PD3_5:1;/* Port P35 direction register */
	 unsigned char	PD3_6:1;/* Port P36 direction register */
	 unsigned char	PD3_7:1;/* Port P37 direction register */
	} BIT;   		        /*        					   */
	unsigned char BYTE;     /*  Byte Access 			   */
};                          /* 						       */

/*------------------------------------------------------
    Port P4 register
------------------------------------------------------*/
union st_p4 {              /* union P4	    	   		    */
	struct {           	   /* Bit  Access 		   	  	    */
	 unsigned char 	P4_0:1;/* Port P40 register			    */
	 unsigned char	P4_1:1;/* Port P41 register 			*/
	 unsigned char	P4_2:1;/* Port P42 register 			*/
	 unsigned char	P4_3:1;/* Port P43 register 			*/
	 unsigned char	P4_4:1;/* Port P44 register 			*/
	 unsigned char	P4_5:1;/* Port P45 register 			*/
	 unsigned char	P4_6:1;/* Port P46 register 			*/
	 unsigned char	P4_7:1;/* Port P47 register 			*/
	} BIT;   		   	   /*        					    */
	unsigned char BYTE;    /*  Byte Access 			        */
};                         /* 						        */

/*------------------------------------------------------
    Port P4 direction register
------------------------------------------------------*/
union st_pd4 {              /* union PD4    	   		   */
	struct {            	/* Bit  Access 		   	  	   */
	 unsigned char 	PD4_0:1;/* Port P40 direction register */
	 unsigned char	PD4_1:1;/* Port P41 direction register */
	 unsigned char	PD4_2:1;/* Port P42 direction register */
	 unsigned char	PD4_3:1;/* Port P43 direction register */
	 unsigned char	PD4_4:1;/* Port P44 direction register */
	 unsigned char	PD4_5:1;/* Port P45 direction register */
	 unsigned char	PD4_6:1;/* Port P46 direction register */
	 unsigned char	PD4_7:1;/* Port P47 direction register */
	} BIT;   		    	/*        					   */
	unsigned char BYTE;    	/*  Byte Access 			   */
};                          /* 						       */

/*------------------------------------------------------
    Port P5 register
------------------------------------------------------*/
union st_p5 {              /* union P5	    	   		    */
	struct {           	   /* Bit  Access 		   	  	    */
	 unsigned char 	P5_0:1;/* Port P50 register			    */
	 unsigned char	P5_1:1;/* Port P51 register 			*/
	 unsigned char	P5_2:1;/* Port P52 register 			*/
	 unsigned char	P5_3:1;/* Port P53 register 			*/
	 unsigned char	P5_4:1;/* Port P54 register 			*/
	 unsigned char	P5_5:1;/* Port P55 register 			*/
	 unsigned char	P5_6:1;/* Port P56 register 			*/
	 unsigned char	P5_7:1;/* Port P57 register 			*/
	} BIT;   		       /*        					    */
	unsigned char BYTE;    /*  Byte Access 			        */
};                         /* 						        */

/*------------------------------------------------------
    Port P5 direction register
------------------------------------------------------*/
union st_pd5 {              /* union PD5    	   		   */
	struct {            	/* Bit  Access 		   	  	   */
	 unsigned char 	PD5_0:1;/* Port P50 direction register */
	 unsigned char	PD5_1:1;/* Port P51 direction register */
	 unsigned char	PD5_2:1;/* Port P52 direction register */
	 unsigned char	PD5_3:1;/* Port P53 direction register */
	 unsigned char	PD5_4:1;/* Port P54 direction register */
	 unsigned char	PD5_5:1;/* Port P55 direction register */
	 unsigned char	PD5_6:1;/* Port P56 direction register */
	 unsigned char	PD5_7:1;/* Port P57 direction register */
	} BIT;   		    	/*        					   */
	unsigned char BYTE;     /*  Byte Access 			   */
};                          /* 						       */

/*------------------------------------------------------
    Port P6 register
------------------------------------------------------*/
union st_p6 {              /* union P6	    	   		    */
	struct {           	   /* Bit  Access 		   	  	    */
	 unsigned char 	P6_0:1;/* Port P60 register			    */
	 unsigned char	P6_1:1;/* Port P61 register 			*/
	 unsigned char	P6_2:1;/* Port P62 register 			*/
	 unsigned char	P6_3:1;/* Port P63 register 			*/
	 unsigned char	P6_4:1;/* Port P64 register 			*/
	 unsigned char	P6_5:1;/* Port P65 register 			*/
	 unsigned char	P6_6:1;/* Port P66 register 			*/
	 unsigned char	P6_7:1;/* Port P67 register 			*/
	} BIT;   		       /*        					    */
	unsigned char BYTE;    /*  Byte Access 			        */
};                         /* 						        */

/*------------------------------------------------------
    Port P6 direction register
------------------------------------------------------*/
union st_pd6 {              /* union PD6    	   		   */
	struct {            	/* Bit  Access 		   	  	   */
	 unsigned char 	PD6_0:1;/* Port P60 direction register */
	 unsigned char	PD6_1:1;/* Port P61 direction register */
	 unsigned char	PD6_2:1;/* Port P62 direction register */
	 unsigned char	PD6_3:1;/* Port P63 direction register */
	 unsigned char	PD6_4:1;/* Port P64 direction register */
	 unsigned char	PD6_5:1;/* Port P65 direction register */
	 unsigned char	PD6_6:1;/* Port P66 direction register */
	 unsigned char	PD6_7:1;/* Port P67 direction register */
	} BIT;   		    	/*        					   */
	unsigned char BYTE;     /*  Byte Access 			   */
};                          /* 						       */

/*------------------------------------------------------
    Port P7 register
------------------------------------------------------*/
union st_p7 {              /* union P7	    	   		    */
	struct {               /* Bit  Access 		   	  	    */
	 unsigned char 	P7_0:1;/* Port P70 register			    */
	 unsigned char	P7_1:1;/* Port P71 register 			*/
	 unsigned char	P7_2:1;/* Port P72 register 			*/
	 unsigned char	P7_3:1;/* Port P73 register 			*/
	 unsigned char	P7_4:1;/* Port P74 register 			*/
	 unsigned char	P7_5:1;/* Port P75 register 			*/
	 unsigned char	P7_6:1;/* Port P76 register 			*/
	 unsigned char	P7_7:1;/* Port P77 register 			*/
	} BIT;   		   	   /*        					    */
	unsigned char BYTE;    /*  Byte Access 			        */
};                         /* 						        */

/*------------------------------------------------------
    Port P7 direction register
------------------------------------------------------*/
union st_pd7 {              /* union PD7    	   		   */
	struct {                /* Bit  Access 		   	  	   */
	 unsigned char 	PD7_0:1;/* Port P70 direction register */
	 unsigned char	PD7_1:1;/* Port P71 direction register */
	 unsigned char	PD7_2:1;/* Port P72 direction register */
	 unsigned char	PD7_3:1;/* Port P73 direction register */
	 unsigned char	PD7_4:1;/* Port P74 direction register */
	 unsigned char	PD7_5:1;/* Port P75 direction register */
	 unsigned char	PD7_6:1;/* Port P76 direction register */
	 unsigned char	PD7_7:1;/* Port P77 direction register */
		} BIT;   		    /*        					   */
	unsigned char BYTE;     /*  Byte Access 			   */
};                          /* 						       */

/*------------------------------------------------------
    Port P8 register
------------------------------------------------------*/
union st_p8 {              /* union P8	    	   		    */
	struct {           	   /* Bit  Access 		   	  	    */
	 unsigned char 	P8_0:1;/* Port P80 register			    */
	 unsigned char	P8_1:1;/* Port P81 register 			*/
	 unsigned char	P8_2:1;/* Port P82 register 			*/
	 unsigned char	P8_3:1;/* Port P83 register 			*/
	 unsigned char	P8_4:1;/* Port P84 register 			*/
	 unsigned char	P8_5:1;/* Port P85 register 			*/
	 unsigned char	P8_6:1;/* Port P86 register 			*/
	 unsigned char	P8_7:1;/* Port P87 register 			*/
	} BIT;   		       /*        					    */
	unsigned char BYTE;    /*  Byte Access 			        */
};                         /* 						        */

/*------------------------------------------------------
    Port P8 direction register
------------------------------------------------------*/
union st_pd8 {              /* union PD8    	   		   */
	struct {            	/* Bit  Access 		   	  	   */
	 unsigned char 	PD8_0:1;/* Port P80 direction register */
	 unsigned char	PD8_1:1;/* Port P81 direction register */
	 unsigned char	PD8_2:1;/* Port P82 direction register */
	 unsigned char	PD8_3:1;/* Port P83 direction register */
	 unsigned char	PD8_4:1;/* Port P84 direction register */
	 unsigned char		 :1;/* Nothing assigned 		   */
	 unsigned char	PD8_6:1;/* Port P86 direction register */
	 unsigned char	PD8_7:1;/* Port P87 direction register */
	} BIT;   		    	/*        					   */
	unsigned char BYTE;     /*  Byte Access 			   */
};                          /* 						       */

/*------------------------------------------------------
    Port P9 register
------------------------------------------------------*/
union st_p9 {              /* union P9	    	   		    */
	struct {           	   /* Bit  Access 		   	  	    */
	 unsigned char 	P9_0:1;/* Port P90 register			    */
	 unsigned char 	P9_1:1;/* Port P91 register 			*/
	 unsigned char	P9_2:1;/* Port P92 register 			*/
	 unsigned char	P9_3:1;/* Port P93 register 			*/
	 unsigned char	P9_4:1;/* Port P94 register 			*/
	 unsigned char	P9_5:1;/* Port P95 register 			*/
	 unsigned char	P9_6:1;/* Port P96 register 			*/
	 unsigned char	P9_7:1;/* Port P97 register 			*/
	} BIT;   		   	   /*        					    */
	unsigned char BYTE;    /*  Byte Access 			        */
};                         /* 						        */

/*------------------------------------------------------
    Port P9 direction register
------------------------------------------------------*/
union st_pd9 {              /* union PD9    	   		   */
	struct {            	/* Bit  Access 		   	  	   */
	 unsigned char 	PD9_0:1;/* Port P90 direction register */
	 unsigned char	PD9_1:1;/* Port P91 direction register */
	 unsigned char	PD9_2:1;/* Port P92 direction register */
	 unsigned char	PD9_3:1;/* Port P93 direction register */
	 unsigned char	PD9_4:1;/* Port P94 direction register */
	 unsigned char	PD9_5:1;/* Port P95 direction register */
	 unsigned char	PD9_6:1;/* Port P96 direction register */
	 unsigned char	PD9_7:1;/* Port P97 direction register */
	} BIT;   		    	/*        					   */
	unsigned char BYTE;     /*  Byte Access 			   */
};                          /* 						       */

/*------------------------------------------------------
    Port P10 register
------------------------------------------------------*/
union st_p10 {              /* union P10	    	   						*/
	struct {            	/* Bit  Access 		   	  	    				*/
	 unsigned char 	P10_0:1;/* Port P100 register							*/
	 unsigned char	P10_1:1;/* Port P101 register 							*/
	 unsigned char	P10_2:1;/* Port P102 register 							*/
	 unsigned char	P10_3:1;/* Port P103 register 							*/
	 unsigned char	P10_4:1;/* Port P104 register 							*/
	 unsigned char	P10_5:1;/* Port P105 register 							*/
	 unsigned char	P10_6:1;/* Port P106 register 							*/
	 unsigned char	P10_7:1;/* Port P107 register 							*/
	} BIT;   		    	/*        					    				*/
	unsigned char BYTE;     /*  Byte Access 			    				*/
};                          /* 						        */

/*------------------------------------------------------
    Port P10 direction register
------------------------------------------------------*/
union st_pd10 {              /* union PD10    	   		     				*/
	struct {            	 /* Bit  Access 		   	  	 				*/
	 unsigned char 	PD10_0:1;/* Port P100 direction register 				*/
	 unsigned char	PD10_1:1;/* Port P101 direction register 				*/
	 unsigned char	PD10_2:1;/* Port P102 direction register 				*/
	 unsigned char	PD10_3:1;/* Port P103 direction register 				*/
	 unsigned char	PD10_4:1;/* Port P104 direction register 				*/
	 unsigned char	PD10_5:1;/* Port P105 direction register 				*/
	 unsigned char	PD10_6:1;/* Port P106 direction register 				*/
	 unsigned char	PD10_7:1;/* Port P107 direction register 				*/
	} BIT;   		    	 /*        					     				*/
	unsigned char BYTE;      /*  Byte Access 			     				*/
};                           /* 						     				*/

/*------------------------------------------------------
    Pull-up control register 0
------------------------------------------------------*/
union st_pur0 {            /* union PUR0    	   		   					*/
	struct {               /* Bit  Access 		   	  	   					*/
	 unsigned char 	PU00:1;/* P00 to P03 pull-up		   					*/
	 unsigned char	PU01:1;/* P04 to P07 pull-up		   					*/
	 unsigned char	PU02:1;/* P10 to P13 pull-up		   					*/
	 unsigned char	PU03:1;/* P14 to P17 pull-up		   					*/
	 unsigned char	PU04:1;/* P20 to P23 pull-up		   					*/
	 unsigned char	PU05:1;/* P24 to P27 pull-up		   					*/
	 unsigned char	PU06:1;/* P30 to P33 pull-up		   					*/
	 unsigned char	PU07:1;/* P34 to P37 pull-up		   					*/
	} BIT;   		       /*        					   					*/
	unsigned char BYTE;    /*  Byte Access 			   	   					*/
};                         /* 						       					*/

/*------------------------------------------------------
    Pull-up control register 1
------------------------------------------------------*/
union st_pur1 {            /* union PUR1    	   		   					*/
	struct {           	   /* Bit  Access 		   	  	   					*/
	 unsigned char 	PU10:1;/* P40 to P43 pull-up 		   					*/
	 unsigned char	PU11:1;/* P44 to P47 pull-up 		   					*/
	 unsigned char	PU12:1;/* P50 to P53 pull-up 		   					*/
	 unsigned char	PU13:1;/* P54 to P57 pull-up 		   					*/
	 unsigned char	PU14:1;/* P60 to P63 pull-up 		   					*/
	 unsigned char	PU15:1;/* P64 to P67 pull-up 		   					*/
	 unsigned char	PU16:1;/* P70 to P73 pull-up (Except P70,P71 ; P70,P71 -> N-channel open drain ports)*/
	 unsigned char	PU17:1;/* P74 to P77 pull-up 		   					*/
	} BIT;   		       /*        					   					*/
	unsigned char BYTE;    /*  Byte Access 			   						*/
};                         /* 						       					*/

/*------------------------------------------------------
    Pull-up control register 2
------------------------------------------------------*/
union st_pur2 {            /* union PUR2    	   		     				*/
	struct {           	   /* Bit  Access 		   	  	     				*/
	 unsigned char 	PU20:1;/* P80 to P83 pull-up 			 				*/
	 unsigned char	PU21:1;/* P84 to P87 pull-up (Except P85)				*/
	 unsigned char	PU22:1;/* P90 to P93 pull-up 			 				*/
	 unsigned char	PU23:1;/* P94 to P97 pull-up			 				*/
	 unsigned char	PU24:1;/* P100 to P103 pull-up 			 				*/
	 unsigned char	PU25:1;/* P104 to P107 pull-up 			 				*/
	 unsigned char		:1;/* Nothing assigned 				 				*/
	 unsigned char		:1;/* Nothing assigned 				 				*/
	} BIT;   		   	   /*        					     				*/
	unsigned char BYTE;    /*  Byte Access 			   		 				*/
};                         /* 						         				*/

/*------------------------------------------------------
    Port control register
------------------------------------------------------*/
union st_pcr {             /* union PCR2    	   		     				*/
	struct {           	   /* Bit  Access 		   	  	     				*/
	 unsigned char 	PCR0:1;/* Port P1 control register 		 				*/
	 unsigned char		:1;/* Nothing assigned 				 				*/
	 unsigned char		:1;/* Nothing assigned 				 				*/
	 unsigned char		:1;/* Nothing assigned 				 				*/
	 unsigned char		:1;/* Nothing assigned 				 				*/
	 unsigned char		:1;/* Nothing assigned 				 				*/
	 unsigned char		:1;/* Nothing assigned 				 				*/
	 unsigned char		:1;/* Nothing assigned 				 				*/
	} BIT;   		   	   /*        					     				*/
	unsigned char BYTE;    /*  Byte Access 			   		 				*/
};                         /* 						         				*/

/*------------------------------------------------------
     bcnic
------------------------------------------------------*/
union st_bcnic {					/*    interrupt control registers*/
    struct{
     unsigned char    ILVL0_BCNIC:1;/* Interrupt priority level select bit */
     unsigned char    ILVL1_BCNIC:1;/* Interrupt priority level select bit */
     unsigned char    ILVL2_BCNIC:1;/* Interrupt priority level select bit */
     unsigned char    IR_BCNIC   :1;/* Interrupt request bit */
     unsigned char    		     :1;
     unsigned char               :1;
     unsigned char               :1;
     unsigned char               :1;
    }BIT;
    unsigned char    BYTE;
};

/*------------------------------------------------------
     dm0ic
------------------------------------------------------*/
union st_dm0ic {					/*    interrupt control registers*/
    struct{
     unsigned char    ILVL0_DM0IC:1;/* Interrupt priority level select bit */
     unsigned char    ILVL1_DM0IC:1;/* Interrupt priority level select bit */
     unsigned char    ILVL2_DM0IC:1;/* Interrupt priority level select bit */
     unsigned char    IR_DM0IC   :1;/* Interrupt request bit			   */
     unsigned char    		     :1;
     unsigned char               :1;
     unsigned char               :1;
     unsigned char               :1;
    }BIT;
    unsigned char    BYTE;
};



/* Processor mode register 0 */
#define PM0   (*(volatile union st_pm0 *)(0x0004))

/* Processor mode register 1 */
#define PM1   (*(volatile union st_pm1 *)(0x0005))

/* System clock control register 0 */
#define CM0   (*(volatile union st_cm0 *)(0x0006))

/* System clock control register 1 */
#define CM1   (*(volatile union st_cm1 *)(0x0007))

/* Chip select control register */
#define CSR   (*(volatile union st_csr *)(0x0008))

/* Address match interrupt enable register */
#define AIER  (*(volatile union st_aier *)(0x0009))

/* Protect register */
#define PRCR  (*(volatile union st_prcr *)(0x000A))

/* Watchdog timer start register */
#define WDTS  (*(volatile unsigned char *)(0x000E))

/* Watchdog timer control register */
#define WDC  (*(volatile union st_wdc *)(0x000F))

/* Address match interrupt register 0 */
#define RMAD0 (*(volatile union st_rmad0 *)(0x0010))

/* Address match interrupt register 1 */
#define RMAD1 (*(volatile union st_rmad1 *)(0x0014))

/* DMA0 source pointer */
#define SAR0  (*(volatile union st_sar0 *)(0x0020))

/* DMA0 destination pointer */
#define DAR0  (*(volatile union st_dar0 *)(0x0024))

/* DMA0 transfer counter */
#define TCR0  (*(volatile union st_tcr0  *)(0x0028))

 /* DMA0 control register */
#define DM0CON (*(volatile union st_dm0con  *)(0x002C))

/* DMA1 source pointer */
#define SAR1  (*(volatile union st_sar1 *)(0x0030))

/* DMA1 destination pointer */
#define DAR1  (*(volatile union st_dar1 *)(0x0034))

 /* DMA1 transfer counter */
#define TCR1  (*(volatile union st_tcr1 *)(0x0038))

/* DMA1 control register */
#define DM1CON  (*(volatile union st_dm1con *)(0x003c))


/* INT3~ interrupt control register */
#define INT3IC  (*(volatile union st_icr *)(0x0044))

/* Timer B5 interrupt control register */
#define TB5IC  (*(volatile union st_icr *)(0x0045))

/* Timer B4 interrupt control register */
#define TB4IC  (*(volatile union st_icr *)(0x0046))

/* Timer B3 interrupt control register */
#define TB3IC  (*(volatile union st_icr *)(0x0047))

/* SI/O4 interrupt control register */
#define S4IC  (*(volatile union st_icr *)(0x0048))

/* SI/O3 interrupt control register */
#define S3IC  (*(volatile union st_icr *)(0x0049))

/* INT5~ interrupt control register */
#define INT5IC  (*(volatile union st_icr *)(0x0048))

/* INT4~ interrupt control register */
#define INT4IC  (*(volatile union st_icr *)(0x0049))

/* Bus collision detection interrupt control register */
#define BCNIC  (*(volatile union st_bcnic *)(0x004a))

/* DMA0 interrupt control register */
#define DM0IC  (*(volatile union st_dm0ic *)(0x004b))

/* DMA1 interrupt control register */
#define DM1IC  (*(volatile union st_icr *)(0x004c))

/* Key input interrupt control register */
#define KUPIC  (*(volatile union st_icr *)(0x004D))

/* A/D conversion interrupt control register */
#define ADIC  (*(volatile union st_icr *)(0x004E))

/* UART2 transmit interrupt control register */
#define S2TIC  (*(volatile union st_icr *)(0x004F))

/* UART2 receive interrupt control register */
#define S2RIC  (*(volatile union st_icr *)(0x0050))

/* UART0 transmit interrupt control register */
#define S0TIC  (*(volatile union st_icr *)(0x0051))

/* UART0 receive interrupt control register */
#define S0RIC  (*(volatile union st_icr *)(0x0052))

/* UART1 transmit interrupt control register */
#define S1TIC (*(volatile union st_icr *)(0x0053))

/* UART1 receive interrupt control register */
#define S1RIC  (*(volatile union st_icr *)(0x0054))

/* Timer A0 interrupt control register */
#define TA0IC  (*(volatile union st_icr *)(0x0055))

/* Timer A1 interrupt control register */
#define TA1IC  (*(volatile union st_icr *)(0x0056))

/* Timer A2 interrupt control register */
#define TA2IC (*(volatile union st_icr *)(0x0057))

/* Timer A3 interrupt control register */
#define TA3IC  (*(volatile union st_icr *)(0x0058))

/* Timer A4 interrupt control register */
#define TA4IC  (*(volatile union st_icr *)(0x0059))

/* Timer B0 interrupt control register */
#define TB0IC  (*(volatile union st_icr *)(0x005A))

/* Timer B1 interrupt control register */
#define TB1IC  (*(volatile union st_icr *)(0x005B))

/* Timer B2 interrupt control register */
#define TB2IC  (*(volatile union st_icr *)(0x005C))

/* INT0~ interrupt control register */
#define INT0IC (*(volatile union st_icr *)(0x005D))

/* INT1~ interrupt control register */
#define INT1IC  (*(volatile union st_icr *)(0x005E))

/* INT2~ interrupt control register */
#define INT2IC  (*(volatile union st_icr *)(0x005F))

/* Timer B3,4,5 count start flag */
#define TBSR  (*(volatile union st_tbsr *)(0x0340))


/********************************************************
*   declare  SFR short                                  *
********************************************************/
/*--------------------------------------------------------
    Timer registers : Read and write data in 16-bit units.
--------------------------------------------------------*/

/* Timer A1-1 register */
#define TA1_1  (*(volatile unsigned short *)(0x0342))

/* Timer A2-1 register */
#define TA2_1  (*(volatile unsigned short *)(0x0344))

/* Timer A4-1 register */
#define TA4_1  (*(volatile unsigned short *)(0x0346))

/* Three-phase PWM control regester 0 */
#define INVC0  (*(volatile union st_invc0 *)(0x0348))

 /* Three-phase PWM control register 1 */
#define INVC1  (*(volatile union st_invc1 *)(0x0349))

/* Three-phase output buffer register 0 */
#define IDB0  (*(volatile union st_idb0 *)(0x034a))

/* Three-phase output buffer register 1 */
#define IDB1  (*(volatile union st_idb1 *)(0x034b))

/*------------------------------------------------------
     Dead time timer ; Use "MOV" instruction when writing to this register.
------------------------------------------------------*/
/* Dead time timer */
#define DTT  (*(volatile unsigned char *)(0x034c))

/*------------------------------------------------------------------
   Timer B2 interrupt occurrences frequency set counter
   ; Use "MOV" instruction when writing to this register.
-------------------------------------------------------------------*/
/* Timer B2 interrupt occurrences frequency set counter */
#define ICTB2  (*(volatile unsigned char *)(0x034d))


/* Timer B3 register */
#define TB3  (*(volatile unsigned short *)(0x0350))

/* Timer B4 register */
#define TB4  (*(volatile unsigned short *)(0x0352))

/* Timer B5 register */
#define TB5  (*(volatile unsigned short *)(0x0354))

/* Timer B3 mode register */
#define TB3MR  (*(volatile union st_tmr *)(0x035b))

/* Timer B4 mode register */
#define TB4MR  (*(volatile union st_tmr *)(0x035c))

/* Timer B5 mode register */
#define TB5MR  (*(volatile union st_tmr *)(0x035d))

/* Interrupt cause select register */
#define IFSR  (*(volatile union st_ifsr *)(0x035f))

/* SI/O3i transmit/receive register (i=3,4)*/
#define S3TRR  (*(volatile unsigned char *)(0x0360))

/* SI/O3 control register */
#define S3C  (*(volatile union st_s3c *)(0x0362))


/* SI/O3 bit rate generator (Use "MOV" instruction when writing to these registers)*/
#define S3BRG  (*(volatile unsigned char *)(0x0363))


/* SI/O4 transmit/receive register */
#define S4TRR  (*(volatile unsigned char *)(0x0364))

/* SI/O4 control register */
#define S4C  (*(volatile union st_s4c *)(0x0366))

/* SI/O4 bit rate generator */
#define S4BRG  (*(volatile unsigned char *)(0x0367))




/* UART2 special mode register 3 */
#define U2SMR3  (*(volatile union st_u2smr3 *)(0x0375))


/* UART2 special mode register 2 */
#define U2SMR2  (*(volatile union st_u2smr2 *)(0x0376))

/* UART2 special mode register */
#define U2SMR  (*(volatile union st_u2smr *)(0x0377))

/* UART2 transmit/receive mode register */
#define U2MR  (*(volatile union st_u2mr *)(0x0378))

/* UART2 bit rate generator */
#define U2BRG  (*(volatile unsigned char *)(0x0379))

/* UART2 transmit buffer register */
#define U2TB  (*(volatile union st_u2tb *)(0x037a))

/* UART2 transmit/receive control register 0 */
//#pragma ADDRESS     u2c0_addr   037cH
#define U2C0  (*(volatile union st_u2c0 *)(0x037c))

/* UART2 transmit/receive control register 1 */
#define U2C1  (*(volatile union st_u2c1 *)(0x037d))

/* UART2 receive buffer register */
#define U2RB  (*(volatile union st_u2rb *)(0x037e))


/* Count start flag */
#define TABSR  (*(volatile union st_tabsr *)(0x0380))

/* Clock prescaler reset flag */
#define CPSRF  (*(volatile union st_cpsrf *)(0x0381))


/* One-shot start flag */
#define ONSF  (*(volatile union st_onsf *)(0x0382))


/* Trigger select register */
#define TRGSR  (*(volatile union st_trgsr *)(0x0383))


/* Up/down flag (Use "MOV" instruction when writing to this register.)*/
#define UDF  (*(volatile unsigned char *)(0x0384))






/* Timer A0 register */
#define TA0 (*(volatile unsigned short  *)(0x0386))

/* Timer A1 register */
#define TA1 (*(volatile unsigned short  *)(0x0388))

/* Timer A2 register */
#define TA2 (*(volatile unsigned short  *)(0x038a))

/* Timer A3 register */
#define TA3 (*(volatile unsigned short  *)(0x038c))

/* Timer A4 register */
#define TA4 (*(volatile unsigned short  *)(0x038e))

/* Timer B0 register */
#define TB0 (*(volatile unsigned short  *)(0x0390))

/* Timer B1 register */
#define TB1 (*(volatile unsigned short  *)(0x0392))

/* Timer B2 register */
#define TB2 (*(volatile unsigned short  *)(0x0394))



 /* Timer A0 mode register */
#define TA0MR  (*(volatile union st_tmr *)(0x0396))

/* Timer A1 mode register */
#define TA1MR  (*(volatile union st_tmr *)(0x0397))

/* Timer A2 mode register */
#define TA2MR  (*(volatile union st_tmr *)(0x0398))

/* Timer A3 mode register */
#define TA3MR  (*(volatile union st_tmr *)(0x0399))

/* Timer A4 mode register */
#define TA4MR  (*(volatile union st_tmr *)(0x039A))

/* Timer B0 mode register */
#define TB0MR  (*(volatile union st_tmr *)(0x039b))

/* Timer B1 mode register */
#define TB1MR  (*(volatile union st_tmr *)(0x039c))

/* Timer B2 mode register */
#define TB2MR  (*(volatile union st_tmr *)(0x039d))



/* UART0 transmit/receive mode register */
#define U0MR  (*(volatile union st_u0mr *)(0x03a0))

/* UART0 bit rate generator (Use "MOV" instruction when writing to these registers.)*/
#define U0BRG  (*(volatile unsigned char *)(0x03a1))

/* UART0 transmit buffer register */
#define U0TB  (*(volatile union st_u0tb *)(0x03a2))

/* UART0 transmit/receive control register 0 */
#define U0C0  (*(volatile union st_u0c0 *)(0x03a4))

 /* UART0 transmit/receive control register 1 */
#define U0C1  (*(volatile union st_u0c1 *)(0x03a5))

/* UART0 receive buffer register */
#define U0RB  (*(volatile union st_u0rb *)(0x03a6))

/* UART1 transmit/receive mode register */
#define U1MR  (*(volatile union st_u1mr *)(0x03a8))

/* UART1 bit rate generator */
#define U1BRG  (*(volatile unsigned char *)(0x03a9))

 /* UART1 transmit buffer register */
#define U1TB  (*(volatile union st_u1tb *)(0x03aa))

/* UART1 transmit/receive control register 0 */
#define U1C0  (*(volatile union st_u1c0 *)(0x03ac))

 /* UART1 transmit/receive control register 1 */
#define U1C1  (*(volatile union st_u1c1 *)(0x03ad))

 /* UART1 receive buffer register */
#define U1RB  (*(volatile union st_u1rb *)(0x03ae))

/* UART transmit/receive control register 2 */
#define UCON  (*(volatile union st_ucon *)(0x03b0))


/* Flash memory control register 1 */
#define FMR1  (*(volatile union st_fmr1 *)(0x03b6))

/* Flash memory control register 0 */
#define FMR0  (*(volatile union st_fmr0 *)(0x03b7))

/* DMA0 cause select register */
#define DM0SL  (*(volatile union st_dm0sl *)(0x03b8))

/* DMA1 cause select register */
#define DM1SL  (*(volatile union st_dm1sl *)(0x03ba))

/* CRC data register */
#define CRCD  (*(volatile union st_crcd *)(0x03bc))

/* CRC input register */
#define CRCIN  (*(volatile union st_crcin*)(0x03be))





/* A/D register 0 */
#define AD0  (*(volatile union st_ad0 *)(0x03c0))

/* A/D register 1 */
#define AD1  (*(volatile union st_ad1 *)(0x03c2))

/* A/D register 2 */
#define AD2  (*(volatile union st_ad2 *)(0x03c4))

/* A/D register 3 */
#define AD3  (*(volatile union st_ad3 *)(0x03c6))

/* A/D register 4 */
#define AD4  (*(volatile union st_ad4 *)(0x03c8))

/* A/D register 5 */
#define AD5  (*(volatile union st_ad5 *)(0x03ca))

/* A/D register 6 */
#define AD6  (*(volatile union st_ad6 *)(0x03cc))

 /* A/D register 7 */
#define AD7  (*(volatile union st_ad7 *)(0x03ce))

/* A/D control register 2 */
#define ADCON2  (*(volatile union st_adcon2 *)(0x03d4))

/* A/D control register 0 */
#define ADCON0  (*(volatile union st_adcon0 *)(0x03d6))

/* A/D control register 1 */
#define ADCON1  (*(volatile union st_adcon1 *)(0x03d7))

/* D/A register 0 */
#define DA0  (*(volatile unsigned char *)(0x03d8))

/* D/A register 1 */
#define DA1  (*(volatile unsigned char *)(0x03da))

/* D/A control register */
#define DACON  (*(volatile union st_dacon *)(0x03dc))

/* Port P0 register */
#define P0  (*(volatile union st_p0 *)(0x03e0))

/* Port P1 register */
#define P1  (*(volatile union st_p1 *)(0x03e1))

/* Port P0 direction register */
#define PD0  (*(volatile union st_pd0 *)(0x03e2))

/* Port P1 direction register */
#define PD1  (*(volatile union st_pd1 *)(0x03e3))

/* Port P2 register */
#define P2  (*(volatile union st_p2 *)(0x03e4))

/* Port P3 register */
#define P3  (*(volatile union st_p3 *)(0x03e5))

/* Port P2 direction register */
#define PD2  (*(volatile union st_pd2 *)(0x03e6))

/* Port P3 direction register */
#define PD3  (*(volatile union st_pd3 *)(0x03e7))


 /* Port P4 register */
#define P4  (*(volatile union st_p4 *)(0x03e8))

/* Port P5 register */
#define P5  (*(volatile union st_p5 *)(0x03e9))

/* Port P4 direction register */
#define PD4  (*(volatile union st_pd4 *)(0x03ea))

/* Port P5 direction register */
#define PD5  (*(volatile union st_pd5 *)(0x03eb))

/* Port P6 register */
#define P6  (*(volatile union st_p6 *)(0x03ec))

/* Port P7 register */
#define P7  (*(volatile union st_p7 *)(0x03ed))

/* Port P6 direction register */
#define PD6  (*(volatile union st_pd6 *)(0x03ee))

/* Port P7 direction register */
#define PD7  (*(volatile union st_pd7 *)(0x03ef))

/* Port P8 register */
#define P8  (*(volatile union st_p8 *)(0x03f0))

/* Port P9 register */
#define P9  (*(volatile union st_p9 *)(0x03f1))

/* Port P8 direction register */
#define PD8  (*(volatile union st_pd8 *)(0x03f2))

/* Port P9 direction register */
#define PD9  (*(volatile union st_pd9 *)(0x03f3))

/* Port P10 register */
#define P10  (*(volatile union st_p10 *)(0x03f4))

/* Port P10 direction register */
#define PD10  (*(volatile union st_pd10 *)(0x03f6))

 /* Pull-up control register 0 */
#define PUR0  (*(volatile union st_pur0 *)(0x03fc))

/* Pull-up control register 1 */
#define PUR1  (*(volatile union st_pur1 *)(0x03fd))

 /* Pull-up control register 2 */
#define PUR2  (*(volatile union st_pur2 *)(0x03fe))

/* Port control register */
#define PCR  (*(volatile union st_pcr *)(0x03ff))

#endif
