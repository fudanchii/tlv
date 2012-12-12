CC=gcc

test: tlv.c test_tlv.c
	@echo "Compiling test program"
	@$(CC) tlv.c test_tlv.c -o test_tlv
	@echo "Done."

