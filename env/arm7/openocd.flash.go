#	OpenOCD Target Script for Atmel AT91SAM7S256
#
#	Programmer: James P Lynch
#

wait_halt							# halt the processor and wait
armv4_5 core_state arm				# select the core state
mww 0xffffff60 0x00320100			# set flash wait state (AT91C_MC_FMR)
mww 0xfffffd44 0xa0008000			# watchdog disable (AT91C_WDTC_WDMR)
mww 0xfffffc20 0xa0000601			# enable main oscillator (AT91C_PMC_MOR)
wait 100							# wait 100 ms
mww 0xfffffc2c 0x00480a0e			# set PLL register (AT91C_PMC_PLLR)
wait 200							# wait 200 ms
mww 0xfffffc30 0x7					# set master clock to PLL (AT91C_PMC_MCKR)
wait 100							# wait 100 ms
mww 0xfffffd08 0xa5000401			# enable user reset AT91C_RSTC_RMR
flash write 0 test.bin 0x0			# program the onchip flash
reset								# reset processor
shutdown							# stop OpenOCD

