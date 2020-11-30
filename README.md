# WeNet

We share net together.
We borrowed a lot of code from [ESPnet](https://github.com/espnet/espnet),
and we refered to [OpenTransformer](https://github.com/ZhengkunTian/OpenTransformer/blob/master/otrans/recognizer.py)
for batch inference.

# Installation

WeNet requries PyTorch 1.6.0+.

``` sh
# 1. setup your own python3 virtual env, miniconda is recommended.
# 2. install pytorch: conda install pytorch torchvision cudatoolkit=10.1 -c pytorch
# 3. install requriments: pip install -r requirements.txt
# 4. link Kaldi on root directory of this repo: ln -s YOUR_KALDI_PATH kaldi
```

# Python code principles

We want that our PyTorch model can be directly exported by torch.jit.script method,
which is essential for deploying the model to production.

See the following resource for how to deploy PyTorch models in production in details.
- [INTRODUCTION TO TORCHSCRIPT](https://pytorch.org/tutorials/beginner/Intro_to_TorchScript_tutorial.html)
- [TORCHSCRIPT LANGUAGE REFERENCE](https://pytorch.org/docs/stable/jit_language_reference.html#language-reference)
- [LOADING A TORCHSCRIPT MODEL IN C++](https://pytorch.org/tutorials/advanced/cpp_export.html)
- [TorchScript and PyTorch JIT | Deep Dive](https://www.youtube.com/watch?v=2awmrMRf0dA&t=574s)
- [Research to Production: PyTorch JIT/TorchScript Updates](https://www.youtube.com/watch?v=St3gdHJzic0)

To ensure that, we will try to export the model before training stage.
If it fails, we should modify the training code to satisfy the export requirements.

``` python
# See in wenet/bin/train.py
script_model = torch.jit.script(model)
script_model.save(os.path.join(args.model_dir, 'init.zip'))
```

Two principles should be taken into consideration when we contribute our python code
to WeNet, especially for the subclass of torch.nn.Module, and for the forward function.

1. Know what is allowed and what is disallowed.
    - [Torch and Tensor Unsupported Attributes](https://pytorch.org/docs/master/jit_unsupported.html#jit-unsupported)
    - [Python Language Reference Coverage](https://pytorch.org/docs/master/jit_python_reference.html#python-language-reference)

2. Try to use explicit typing as much as possible. You can try to do type checking
forced by typeguard, see https://typeguard.readthedocs.io/en/latest/userguide.html for details.

