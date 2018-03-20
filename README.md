# ceph_s3_tests
Some rewrites of S3 Compability tests (that where in Python) for Ceph in C++

This is a set of completely unofficial Amazon AWS S3 compatibility tests, that will hopefully be useful to people implementing software that exposes an S3-like API.

The tests only cover the REST interface.

These tests use the gtest framework and the AWS SDK for C++ library and should be built using CMake.

The CMake should take care of gtest and the AWS SDK, so it's not needed to perform the instalations. You should, however have CMake to use this project.

This project also requires some configuration to establish the S3 connection properly, one of the ways of doing that is by installing the AWS CLI and filling the four pieces of information that are prompted.
`$ aws configure
AWS Access Key ID [None]: EXAMPLEFODNN7EXAMPLE
AWS Secret Access Key [None]: EXAMPLEtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY
Default region name [None]: us-east-1
Default output format [None]: json`


