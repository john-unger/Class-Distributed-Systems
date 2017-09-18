default: pi dpi tpi

pi: pi_serial.chpl
	chpl --fast -o $@ $<

dpi: pi_datapar.chpl
	chpl --fast -o $@ $<

tpi: pi_taskpar.chpl
	chpl --fast -o $@ $<

clean:
	rm -f pi dpi tpi

