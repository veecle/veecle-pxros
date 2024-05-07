SECTIONS
{
  /*
   * Memory allocation for Ethernet driver task
   * (RX and TX data buffers)
   */
  .eth_mem : ALIGN(32)
  {
      . = ALIGN(8) ;
      PX_ETH_DATA_BASE = . ;
      *(.eth_descriptors)
      *libilld.a*:*(.bss)
      *GETH*(.bss)
      . = ALIGN(8) ;
      PX_ETH_DATA_END = . ;
  } > CORE_SYM(ETH_MEM)

  CORE_SEC(.clear_sec) :
  {
    LONG(0 + ADDR(.eth_mem));
	LONG(SIZEOF(.eth_mem));
  } > RODATA_MEM
}
