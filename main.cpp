#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/Bucket.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/Object.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/core/utils/logging/DefaultLogSystem.h>
#include <aws/core/utils/logging/AWSLogging.h>

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

Aws::String get_new_bucket(Aws::S3::S3Client * s3_client, Aws::String bucket_name){
    Aws::S3::Model::CreateBucketRequest request;
    request.SetBucket(bucket_name);

    auto outcome = s3_client->CreateBucket(request);
    if (outcome.IsSuccess())
    {
        std::cout << "Done!" << std::endl;
    }
    else
    {
        std::cout << "CreateBucket error: " << bucket_name
                  << outcome.GetError().GetExceptionName() << std::endl
                  << outcome.GetError().GetMessage() << std::endl;
    }
    return bucket_name;
}

class S3Tests : public ::testing::Test {
protected:
    Aws::S3::S3Client * s3_client;
    Aws::SDKOptions * options;

    virtual void SetUp() {
        options = new Aws::SDKOptions();
        // options->loggingOptions.logLevel =  Aws::Utils::Logging::LogLevel::Trace;
        Aws::InitAPI(*options);

        Aws::Client::ClientConfiguration clientconfig;
        clientconfig.region = Aws::Region::EU_WEST_1;

        clientconfig.scheme = Aws::Http::Scheme::HTTP;
        std::string endpoint = "localhost:8000";
        clientconfig.endpointOverride = endpoint.c_str();

        // S3 client on the CPP Sdk uses bucketname.localhost:8000 by default
        // That would require some some crazy DNS configs locally to work
        // like suggested here https://groups.google.com/d/msg/leoproject_leofs/bHSIS3Uh1lk/gYiQVOlkBAAJ
        // 
        // Instead, let's short-circuit this conditional to false to use localhost:8000/bucketname
        // https://github.com/aws/aws-sdk-cpp/blob/bbf1cd2dd9884551a9637cf50b4c85ef1afeeead/aws-cpp-sdk-s3/source/S3Client.cpp#L2766
        s3_client = new Aws::S3::S3Client(clientconfig,
                                          Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy::Never,
                                          false);
    }

    virtual void TearDown() {
        Aws::ShutdownAPI(*options);
        delete s3_client; s3_client = nullptr;
        delete options; options = nullptr;
    }
};

TEST_F(S3Tests, S3Tests_NewBucketIsEmpty_Test) {
    Aws::String bucket;
    bucket = get_new_bucket(s3_client, get_random_bucket_name());

    // --
    Aws::S3::Model::ListObjectsRequest objects_request;
    objects_request.WithBucket(bucket);

    auto list_objects_outcome = s3_client->ListObjects(objects_request);

    if (list_objects_outcome.IsSuccess())
    {
        Aws::Vector<Aws::S3::Model::Object> object_list =
                list_objects_outcome.GetResult().GetContents();

        for (auto const &s3_object : object_list)
        {
            std::cout << "* " << s3_object.GetKey() << std::endl;
        }
    }
    else
    {
        std::cout << "ListObjects error: " <<
                  list_objects_outcome.GetError().GetExceptionName() << " " <<
                  list_objects_outcome.GetError().GetMessage() << std::endl;
    }
}

TEST_F(S3Tests, S3Tests_Failing_Test) {
//    throw 20;
}
int main(int argc, char* argv[]) {
    Aws::Utils::Logging::InitializeAWSLogging(
            Aws::MakeShared<Aws::Utils::Logging::DefaultLogSystem>(
                    "RunUnitTests", Aws::Utils::Logging::LogLevel::Trace, "aws_sdk_"));
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    std::cout << "Hello, World!" << std::endl;
    Aws::Utils::Logging::ShutdownAWSLogging();
    return 0;
}