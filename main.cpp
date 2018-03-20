#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/Bucket.h>
#include <aws/s3/model/CreateBucketRequest.h>

#include <random>   //for the bucket name generator
//#include <iostream>

int get_random_number(int a, int b) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(a, b);

    return dis(gen);
}

Aws::String get_random_bucket_name() {
    //creates a random s3 bucket name from 3 to 63 chars with numbers and lower letters
    //dashes and periods have some constraints to them, so I thought it would be easier
    //to just leave them out
    int len;
    Aws::String name;
    char possibilities[37] = "0123456789abcdefghijklmnopqrstuvwxyz";

    len = get_random_number(3, 63);
    name = "";

    for (int i=0; i<len; i++){
        name += possibilities[get_random_number(0,35)];
    }

    return name;
}

int get_new_bucket(Aws::S3::S3Client s3_client, Aws::String bucket_name){
    Aws::S3::Model::CreateBucketRequest request;
    request.SetBucket(bucket_name);

    auto outcome = s3_client.CreateBucket(request);
    if (outcome.IsSuccess())
    {
        std::cout << "Done!" << std::endl;
        return 1;
    }
    else
    {
        std::cout << "CreateBucket error: " << bucket_name
                  << outcome.GetError().GetExceptionName() << std::endl
                  << outcome.GetError().GetMessage() << std::endl;
        return 0;
    }
}



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

    //tests here


    while (get_new_bucket(s3_client, get_random_bucket_name()) != 1);


    Aws::ShutdownAPI(options);
    return 0;
}