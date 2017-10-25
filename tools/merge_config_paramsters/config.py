#!/usr/bin/env python
# coding=utf-8

from mobilenet import mobile_net

NET_OUT = mobile_net(3*224*224, #input data size
                     102, #class num (flowers 102)
                     1.0 # mobilenet's scale 
                    )

MODEL_PARAM = './mobilenet_flowers102.tar.gz'

# the file we generate
MERGED_FILE = './mobilenet.paddle'
