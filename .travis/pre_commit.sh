#!/bin/bash
function abort(){
    echo "Your commit not fit PaddlePaddle code style" 1>&2
    echo "Please use pre-commit scripts to auto-format your code" 1>&2
    echo "Install pre-commit following 2 steps:" 1>&2
    echo "1> pip install pre-commit" 1>&2
    echo "2> pre-commit install (under the Mobile repo)" 1>&2
    exit 1
}

trap 'abort' 0
set -e
cd `dirname $0`
cd ..
export PATH=/usr/bin:$PATH
pre-commit install

if ! pre-commit run -a ; then
  ls -lh
  git diff  --exit-code
  exit 1
fi

trap : 0
