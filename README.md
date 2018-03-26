# ceph_s3_tests
Some rewrites of S3 Compatibility tests (that were in Python) for Ceph in C++

This is a set of completely unofficial Amazon AWS S3 compatibility tests, that will hopefully be useful to people implementing software that exposes an S3-like API.

The tests only cover the REST interface.

## Installing

These tests use the gtest framework and the AWS SDK for C++ library and should be built using CMake.
For this to work, you will need all of the following.

* Install the requirements for [GoogleTest](https://github.com/google/googletest#requirements)

* Install the prerequisites and dependencies for [AWS SDK for C++](https://docs.aws.amazon.com/sdk-for-cpp/v1/developer-guide/setup.html)

* Install [CMake](https://cmake.org/install/)


The CMake should take care of downloading and setting up a local copy of gtest and the AWS SDK, so it's not needed to perform the installations.

This project also requires some configuration to establish the S3 connection properly, one of the ways of doing that is by installing the [AWS CLI](https://docs.aws.amazon.com/cli/latest/userguide/installing.html) and running the following command (and then filling the prompts that will follow).

    $ aws configure
    AWS Access Key ID [None]: XAMPLEFODNN7EXAMPLE
    AWS Secret Access Key [None]: EXAMPLEtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY
    Default region name [None]: us-east-1
    Default output format [None]: xml

## Building

Clone this repository.
Run `cmake ./` on the repository root.

## Running

After compiling, you should be able to run `cmake-build-debug/ceph_s3_tests`
