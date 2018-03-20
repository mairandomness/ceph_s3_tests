#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/Bucket.h>
#include <aws/s3/model/CreateBucketRequest.h>

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    std::cout << "Hello, World!" << std::endl;

    Aws::SDKOptions options;
    Aws::InitAPI(options);

    Aws::Client::ClientConfiguration clientconfig;
    clientconfig.region = "mexico";
    clientconfig.scheme = Aws::Http::Scheme::HTTP;
    std::string endpoint = "localhost:8000";
    clientconfig.endpointOverride = endpoint.c_str();

    Aws::S3::S3Client s3_client(clientconfig);

    // the tests


    Aws::ShutdownAPI(options);
    return 0;
}