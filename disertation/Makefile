CONTIKI = ../..

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

%.o: %.c definitions.h
	gcc -Wall -c -o $@ $<

all: communication.o definitions.o
	gcc communication.c


#all: example-abc example-mesh example-collect example-trickle example-polite \
#     example-rudolph0 example-rudolph1 example-rudolph2 example-rucb \
#     example-runicast example-unicast example-neighbors
#CFLAGS+= -DNM
# include $(CONTIKI)/Makefile.include
