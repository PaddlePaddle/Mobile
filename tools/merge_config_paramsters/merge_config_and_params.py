#!/usr/bin/env python
# coding=utf-8

import gzip
import struct
import os

import paddle.v2 as paddle
from paddle.proto import ModelConfig_pb2
from paddle.v2.topology import Topology

from config import NET_OUT, MODEL_PARAM, MERGED_FILE

class Merge_model:
    def __init__(self):
        self.model_file = MODEL_PARAM
        self.merged_file = MERGED_FILE
        self.net = NET_OUT
        self.load_conf()
        self.load_params()

    def load_conf(self):
        topo = Topology(self.net)
        self.conf = topo.proto()
        assert isinstance(self.conf, ModelConfig_pb2.ModelConfig)

    def load_params(self):
        with gzip.open(self.model_file) as f: 
            self.params = paddle.parameters.Parameters.from_tar(f) 

    def do_merge(self):
        if os.path.exists(self.merged_file):
            os.remove(self.merged_file)
        with open(self.merged_file, 'w') as f:
            param_names = [param.name for param in self.conf.parameters]
            conf_str = self.conf.SerializeToString()
            f.write(struct.pack('q', len(conf_str)))
            f.write(conf_str)
            for pname in param_names:
                self.params.serialize(pname, f)

if __name__ == '__main__':

    Merge_model().do_merge()

    print 'Generate ', MERGED_FILE
    print '\nMerge SUCCESS!'
