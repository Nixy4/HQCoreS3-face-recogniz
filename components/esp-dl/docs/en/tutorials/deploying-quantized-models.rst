Manual Deployment Guide for pre-quantized Models
====================================================

:link_to_translation:`zh_CN:[中文]`

This tutorial shows how to customize a model with our :doc:`convert tool <../tools/convert-tool/usage-of-convert-tool>`. In this tutorial, the example is a runnable project about `MNIST <https://tensorflow.google.cn/datasets/catalog/mnist?hl=en>`__ classification mission, hereinafter referred to as MNIST.

Note:

- If your model has already been quantized on other platforms (e.g. TFLite int8 model), and the quantization scheme is different from that of ESP-DL (see :doc:`quantization specification <../tools/quantization-toolkit/quantization-specification>`), then the model cannot be deployed with ESP-DL.
- If your model has not been quantized yet, you can deploy it with reference to :doc:`deploying-models`.

It is recommended to learn post-training quantization first.

For how to customize a layer, please check :doc:`customizing-a-layer-step-by-step`.

Below is the structure of this tutorial project.

.. code:: none

   tutorial/
   ├── CMakeLists.txt
   ├── main
   │   ├── app_main.cpp
   │   └── CMakeLists.txt
   ├── model
   │   ├── mnist_coefficient.cpp   (generated in Step 3)
   │   ├── mnist_coefficient.hpp   (generated in Step 3)
   │   ├── mnist_model.hpp
   │   └── npy
   │       ├── config.json
   │       ├── l1_bias.npy
   │       ├── l1_filter.npy
   │       ├── l2_compress_bias.npy
   │       ├── l2_compress_filter.npy
   │       ├── l2_depth_filter.npy
   │       ├── l3_a_compress_bias.npy
   │       ├── l3_a_compress_filter.npy
   │       ├── l3_a_depth_filter.npy
   │       ├── l3_b_compress_bias.npy
   │       ├── l3_b_compress_filter.npy
   │       ├── l3_b_depth_filter.npy
   │       ├── l3_c_compress_bias.npy
   │       ├── l3_c_compress_filter.npy
   │       ├── l3_c_depth_filter.npy
   │       ├── l3_d_compress_bias.npy
   │       ├── l3_d_compress_filter.npy
   │       ├── l3_d_depth_filter.npy
   │       ├── l3_e_compress_bias.npy
   │       ├── l3_e_compress_filter.npy
   │       ├── l3_e_depth_filter.npy
   │       ├── l4_compress_bias.npy
   │       ├── l4_compress_filter.npy
   │       ├── l4_depth_activation.npy
   │       ├── l4_depth_filter.npy
   │       ├── l5_compress_bias.npy
   │       ├── l5_compress_filter.npy
   │       ├── l5_depth_activation.npy
   │       └── l5_depth_filter.npy
   └── README.md

Step 1: Save Model Coefficients
-------------------------------

Save floating-point coefficients of your model in .npy format using the `numpy.save () <https://numpy.org/doc/stable/reference/generated/numpy.save.html?highlight=save#numpy.save>`__ function:

::

   numpy.save(file=f'{filename}', arr=coefficient)

For each layer of a neural network operation, you might need:

-  **filter**: saved as ``'{layer_name}_filter.npy'``
-  **bias**: saved as ``'{layer_name}_bias.npy'``
-  **activation**: activation functions with coefficients such as *LeakyReLU* and *PReLU*, saved as ``'{layer_name}_activation.npy'``

**Example**: coefficients of the MNIST project saved into .npy files in :project:`tutorial/convert_tool_example/model/npy/`.

Step 2: Write Model Configuration
---------------------------------

Write model configuration in the config.json file following the :doc:`../tools/convert-tool/specification-of-config-json`.

**Example**: configuration of the MNIST project saved in :project_file:`tutorial/convert_tool_example/model/npy/config.json`.

.. _convert-model_coefficients:

Step 3: Convert Model Coefficients
----------------------------------

Once the coefficient.npy files and config.json file are ready and stored in the same folder, convert the coefficients into C/C++ code using convert.py (see instructions in :doc:`../tools/convert-tool/usage-of-convert-tool`).

**Example**:

Run

.. code:: bash

   python ../tools/convert_tool/convert.py -t [SoC] -i ./model/npy/ -n mnist_coefficient -o ./model/

and two files ``mnist_coefficient.cpp`` and ``mnist_coefficient.hpp`` would be generated in :project:`tutorial/convert_tool_example/model`.

Then, the coefficients of each layer could be fetched by calling ``get_{layer_name}_***()``. For example, get the filter of “l1” by calling ``get_l1_filter()``.

Step 4: Build a Model
---------------------

Step 4.1: Derive a class from the Model class in :project_file:`dl_layer_model.hpp <include/layer/dl_layer_model.hpp>`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: none

   class MNIST : public Model<int16_t>
   {
   };

Step 4.2: Declare layers as member variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: none

   class MNIST : public Model<int16_t>
   {
   private:
       Conv2D<int16_t> l1;                  // a layer named l1
       DepthwiseConv2D<int16_t> l2_depth;   // a layer named l2_depth
       Conv2D<int16_t> l2_compress;         // a layer named l2_compress
       DepthwiseConv2D<int16_t> l3_a_depth; // a layer named l3_a_depth
       Conv2D<int16_t> l3_a_compress;       // a layer named l3_a_compress
       DepthwiseConv2D<int16_t> l3_b_depth; // a layer named l3_b_depth
       Conv2D<int16_t> l3_b_compress;       // a layer named l3_b_compress
       DepthwiseConv2D<int16_t> l3_c_depth; // a layer named l3_c_depth
       Conv2D<int16_t> l3_c_compress;       // a layer named l3_c_compress
       DepthwiseConv2D<int16_t> l3_d_depth; // a layer named l3_d_depth
       Conv2D<int16_t> l3_d_compress;       // a layer named l3_d_compress
       DepthwiseConv2D<int16_t> l3_e_depth; // a layer named l3_e_depth
       Conv2D<int16_t> l3_e_compress;       // a layer named l3_e_compress
       Concat2D<int16_t> l3_concat;         // a layer named l3_concat
       DepthwiseConv2D<int16_t> l4_depth;   // a layer named l4_depth
       Conv2D<int16_t> l4_compress;         // a layer named l4_compress
       DepthwiseConv2D<int16_t> l5_depth;   // a layer named l5_depth

   public:
       Conv2D<int16_t> l5_compress; // a layer named l5_compress. Make the l5_compress public, as the l5_compress.get_output() will be fetched outside the class.
   };

Step 4.3: Initialize layers in constructor function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Initialize layers with the coefficients in ``"mnist_coefficient.hpp"`` generated in :ref:`convert-model_coefficients`.

For how to initialize each Layer, please check the corresponding .hpp file in :project:`include/layer/`.

.. code:: none

   class MNIST : public Model<int16_t>
   {
       // ellipsis member variables

       MNIST() : l1(Conv2D<int16_t>(-2, get_l1_filter(), get_l1_bias(), get_l1_activation(), PADDING_VALID, {}, 2, 2, "l1")),
                 l2_depth(DepthwiseConv2D<int16_t>(-1, get_l2_depth_filter(), NULL, get_l2_depth_activation(), PADDING_SAME_END, {}, 2, 2, "l2_depth")),
                 l2_compress(Conv2D<int16_t>(-3, get_l2_compress_filter(), get_l2_compress_bias(), NULL, PADDING_SAME_END, {}, 1, 1, "l2_compress")),
                 l3_a_depth(DepthwiseConv2D<int16_t>(-1, get_l3_a_depth_filter(), NULL, get_l3_a_depth_activation(), PADDING_VALID, {}, 1, 1, "l3_a_depth")),
                 l3_a_compress(Conv2D<int16_t>(-12, get_l3_a_compress_filter(), get_l3_a_compress_bias(), NULL, PADDING_VALID, {}, 1, 1, "l3_a_compress")),
                 l3_b_depth(DepthwiseConv2D<int16_t>(-2, get_l3_b_depth_filter(), NULL, get_l3_b_depth_activation(), PADDING_VALID, {}, 1, 1, "l3_b_depth")),
                 l3_b_compress(Conv2D<int16_t>(-12, get_l3_b_compress_filter(), get_l3_b_compress_bias(), NULL, PADDING_VALID, {}, 1, 1, "l3_b_compress")),
                 l3_c_depth(DepthwiseConv2D<int16_t>(-12, get_l3_c_depth_filter(), NULL, get_l3_c_depth_activation(), PADDING_SAME_END, {}, 1, 1, "l3_c_depth")),
                 l3_c_compress(Conv2D<int16_t>(-12, get_l3_c_compress_filter(), get_l3_c_compress_bias(), NULL, PADDING_SAME_END, {}, 1, 1, "l3_c_compress")),
                 l3_d_depth(DepthwiseConv2D<int16_t>(-12, get_l3_d_depth_filter(), NULL, get_l3_d_depth_activation(), PADDING_SAME_END, {}, 1, 1, "l3_d_depth")),
                 l3_d_compress(Conv2D<int16_t>(-11, get_l3_d_compress_filter(), get_l3_d_compress_bias(), NULL, PADDING_SAME_END, {}, 1, 1, "l3_d_compress")),
                 l3_e_depth(DepthwiseConv2D<int16_t>(-11, get_l3_e_depth_filter(), NULL, get_l3_e_depth_activation(), PADDING_SAME_END, {}, 1, 1, "l3_e_depth")),
                 l3_e_compress(Conv2D<int16_t>(-12, get_l3_e_compress_filter(), get_l3_e_compress_bias(), NULL, PADDING_SAME_END, {}, 1, 1, "l3_e_compress")),
                 l3_concat(-1, "l3_concat"),
                 l4_depth(DepthwiseConv2D<int16_t>(-12, get_l4_depth_filter(), NULL, get_l4_depth_activation(), PADDING_VALID, {}, 1, 1, "l4_depth")),
                 l4_compress(Conv2D<int16_t>(-11, get_l4_compress_filter(), get_l4_compress_bias(), NULL, PADDING_VALID, {}, 1, 1, "l4_compress")),
                 l5_depth(DepthwiseConv2D<int16_t>(-10, get_l5_depth_filter(), NULL, get_l5_depth_activation(), PADDING_VALID, {}, 1, 1, "l5_depth")),
                 l5_compress(Conv2D<int16_t>(-9, get_l5_compress_filter(), get_l5_compress_bias(), NULL, PADDING_VALID, {}, 1, 1, "l5_compress")) {}

   };

Step 4.4: Implement ``void build(Tensor<input_t> &input)``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To distinguish ``build()`` of ``Model`` and ``build()`` of ``Layer``, we define:

-  ``Model.build()`` as ``build()`` of ``Model``;
-  ``Layer.build()`` as ``build()`` of ``Layer``.

In ``Model.build()``, all ``Layer.build()`` are called. ``Model.build()`` is effective when input shape changes. If input shape does not change, ``Model.build()`` will not be called, thus saving computing time.

For when ``Model.build()`` is called, please check :ref:`run-model`.

For how to call ``Layer.build()`` of each layer, please refer to the corresponding .hpp file in :project:`include/layer/`.

.. code:: none

   class MNIST : public Model<int16_t>
   {
       // ellipsis member variables
       // ellipsis constructor function

       void build(Tensor<int16_t> &input)
       {
           this->l1.build(input);
           this->l2_depth.build(this->l1.get_output());
           this->l2_compress.build(this->l2_depth.get_output());
           this->l3_a_depth.build(this->l2_compress.get_output());
           this->l3_a_compress.build(this->l3_a_depth.get_output());
           this->l3_b_depth.build(this->l2_compress.get_output());
           this->l3_b_compress.build(this->l3_b_depth.get_output());
           this->l3_c_depth.build(this->l3_b_compress.get_output());
           this->l3_c_compress.build(this->l3_c_depth.get_output());
           this->l3_d_depth.build(this->l3_b_compress.get_output());
           this->l3_d_compress.build(this->l3_d_depth.get_output());
           this->l3_e_depth.build(this->l3_d_compress.get_output());
           this->l3_e_compress.build(this->l3_e_depth.get_output());
           this->l3_concat.build({&this->l3_a_compress.get_output(), &this->l3_c_compress.get_output(), &this->l3_e_compress.get_output()});
           this->l4_depth.build(this->l3_concat.get_output());
           this->l4_compress.build(this->l4_depth.get_output());
           this->l5_depth.build(this->l4_compress.get_output());
           this->l5_compress.build(this->l5_depth.get_output());
       }
   };

Step 4.5: Implement ``void call(Tensor<input_t> &input)``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In ``Model.call()``, all ``Layer.call()`` are called. For how to call ``Layer.call()`` of each layer, please refer to the corresponding .hpp file in :project:`include/layer/`.

.. code:: none

   class MNIST : public Model<int16_t>
   {
       // ellipsis member variables
       // ellipsis constructor function
       // ellipsis build(...)

       void call(Tensor<int16_t> &input)
       {
           this->l1.call(input);
           input.free_element();

           this->l2_depth.call(this->l1.get_output());
           this->l1.get_output().free_element();

           this->l2_compress.call(this->l2_depth.get_output());
           this->l2_depth.get_output().free_element();

           this->l3_a_depth.call(this->l2_compress.get_output());
           // this->l2_compress.get_output().free_element();

           this->l3_a_compress.call(this->l3_a_depth.get_output());
           this->l3_a_depth.get_output().free_element();

           this->l3_b_depth.call(this->l2_compress.get_output());
           this->l2_compress.get_output().free_element();

           this->l3_b_compress.call(this->l3_b_depth.get_output());
           this->l3_b_depth.get_output().free_element();

           this->l3_c_depth.call(this->l3_b_compress.get_output());
           // this->l3_b_compress.get_output().free_element();

           this->l3_c_compress.call(this->l3_c_depth.get_output());
           this->l3_c_depth.get_output().free_element();

           this->l3_d_depth.call(this->l3_b_compress.get_output());
           this->l3_b_compress.get_output().free_element();

           this->l3_d_compress.call(this->l3_d_depth.get_output());
           this->l3_d_depth.get_output().free_element();

           this->l3_e_depth.call(this->l3_d_compress.get_output());
           this->l3_d_compress.get_output().free_element();

           this->l3_e_compress.call(this->l3_e_depth.get_output());
           this->l3_e_depth.get_output().free_element();

           this->l3_concat.call({&this->l3_a_compress.get_output(), &this->l3_c_compress.get_output(), &this->l3_e_compress.get_output()}, true);

           this->l4_depth.call(this->l3_concat.get_output());
           this->l3_concat.get_output().free_element();

           this->l4_compress.call(this->l4_depth.get_output());
           this->l4_depth.get_output().free_element();

           this->l5_depth.call(this->l4_compress.get_output());
           this->l4_compress.get_output().free_element();

           this->l5_compress.call(this->l5_depth.get_output());
           this->l5_depth.get_output().free_element();
       }
   };

.. _run-model:

Step 5: Run a Model
-------------------

-  Create an object of Model

-  Run ``Model.forward()`` for neural network inference. The progress of ``Model.forward()`` is:

   .. code:: none

      forward()
      {
        if (input_shape is changed)
        {
            Model.build();
        }
        Model.call();
      }

**Example**: The object of MNIST and the ``forward()`` function in :project_file:`tutorial/convert_tool_example/main/app_main.cpp`.

.. code:: none

   // model forward
   MNIST model;
   model.forward(input);
