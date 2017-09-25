# edit-mode: -*- python -*-
import paddle.v2 as paddle


def conv_bn_layer(input, filter_size, num_filters,
                  stride, padding, channels=None, num_groups=1,
                  active_type=paddle.activation.Relu()):
    """
    A wrapper for conv layer with batch normalization layers.
    Note:
    conv layer has no activation.
    """
    tmp = paddle.layer.img_conv(
                         input=input,
                         filter_size=filter_size,
                         num_channels=channels,
                         num_filters=num_filters,
                         stride=stride,
                         padding=padding,
                         groups=num_groups,
                         #act=active_type,
                         act=paddle.activation.Linear(),
                         bias_attr=False)
                         #act=active_type,
                         #bias_attr=True)
    return paddle.layer.batch_norm(
                            input=tmp,
                            act=active_type)
                            #use_global_stats=False,
def depthwise_separable(input, num_filters1, num_filters2, num_groups, stride):
    tmp = conv_bn_layer(
                        input=input,
                        filter_size=3,
                        num_filters=num_filters1,
                        stride=stride,
                        padding=1,
                        num_groups=num_groups)

    tmp = conv_bn_layer(
                        input=tmp,
                        filter_size=1,
                        num_filters=num_filters2,
                        stride=1,
                        padding=0)
    return tmp

def mobile_net(datadim, classdim):

    img = paddle.layer.data(
        name="image", type=paddle.data_type.dense_vector(datadim))

    # conv1: 112x112
    tmp = conv_bn_layer(img,
                        filter_size=3,
                        channels=3,
                        num_filters=32,
                        stride=2,
                        padding=1)

    # 56x56
    tmp = depthwise_separable(tmp,
                              num_filters1=32,
                              num_filters2=64,
                              num_groups=32,
                              stride=1)
    tmp = depthwise_separable(tmp,
                              num_filters1=64,
                              num_filters2=128,
                              num_groups=64,
                              stride=2)
    # 28x28
    tmp = depthwise_separable(tmp,
                              num_filters1=128,
                              num_filters2=128,
                              num_groups=128,
                              stride=1)
    tmp = depthwise_separable(tmp,
                              num_filters1=128,
                              num_filters2=256,
                              num_groups=128,
                              stride=2)
    # 14x14
    tmp = depthwise_separable(tmp,
                              num_filters1=256,
                              num_filters2=256,
                              num_groups=256,
                              stride=1)
    tmp = depthwise_separable(tmp,
                              num_filters1=256,
                              num_filters2=512,
                              num_groups=256,
                              stride=2)
    # 14x14
    for i in range(5):
        tmp = depthwise_separable(tmp,
                                  num_filters1=512,
                                  num_filters2=512,
                                  num_groups=512,
                                  stride=1)
    # 7x7
    tmp = depthwise_separable(tmp,
                              num_filters1=512,
                              num_filters2=1024,
                              num_groups=512,
                              stride=2)
    tmp = depthwise_separable(tmp,
                              num_filters1=1024,
                              num_filters2=1024,
                              num_groups=1024,
                              stride=1)

    tmp = paddle.layer.img_pool(
                         input=tmp,
                         pool_size=7,
                         stride=1,
                         pool_type=paddle.pooling.Avg())

    out = paddle.layer.fc(
        input=tmp, size=classdim, act=paddle.activation.Softmax())

    return out
