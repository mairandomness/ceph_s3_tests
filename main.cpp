#include <iostream>
#include <fstream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/Bucket.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/Object.h>
#include <random>   //for the bucket name generator

int get_random_number(int a, int b) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(a, b);

    return dis(gen);
}

Aws::String get_random_bucket_name() {
    //creates a random s3 bucket name from 3 to 63 chars with numbers and lower letters
    //dashes and periods have some constraints to them, so I thought it would be easier
    //to just leave them out for now. Since these tests don't use prefix, we left that idea out for simplicity
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

Aws::S3::Model::CreateBucketOutcome get_new_bucket(Aws::S3::S3Client * s3_client, Aws::String bucket_name) {
    Aws::S3::Model::CreateBucketRequest request;
    request.SetBucket(bucket_name);

    auto outcome = s3_client->CreateBucket(request);

    return outcome;
}

Aws::S3::Model::ListObjectsOutcome list_objects(Aws::S3::S3Client * s3_client, Aws::String bucket_name) {
    Aws::S3::Model::ListObjectsRequest objects_request;
    objects_request.WithBucket(bucket_name);

    auto list_objects_outcome = s3_client->ListObjects(objects_request);

    return list_objects_outcome;
}

Aws::S3::Model::PutObjectOutcome put_new_object(Aws::S3::S3Client * s3_client, Aws::String bucket_name, Aws::String key_name, Aws::String file_name) {
    Aws::S3::Model::PutObjectRequest object_request;
    object_request.WithBucket(bucket_name).WithKey(key_name);

    auto input_data = Aws::MakeShared<Aws::FStream>("PutObjectInputStream",
        file_name.c_str(), std::ios_base::in | std::ios_base::binary);
    object_request.SetBody(input_data);

    auto put_object_outcome = s3_client->PutObject(object_request);

    return put_object_outcome;
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
    auto bucket = get_random_bucket_name();

    EXPECT_TRUE(get_new_bucket(s3_client, bucket).IsSuccess());

    auto list_objects_outcome = list_objects(s3_client, bucket);

    EXPECT_TRUE(list_objects_outcome.IsSuccess());

    auto objects = list_objects_outcome.GetResult().GetContents();

    EXPECT_EQ(0, objects.size());
}


TEST_F(S3Tests, S3Tests_BucketListDistinct__Test) {
//this test is a bit different than the version in python, just a bit more through,
//we are also testing that the bucket that had an object put in it has one object.
    auto bucket1 = get_random_bucket_name();
    auto bucket2 = get_random_bucket_name();

    EXPECT_TRUE(get_new_bucket(s3_client, bucket1).IsSuccess());
    EXPECT_TRUE(get_new_bucket(s3_client, bucket2).IsSuccess());

    std::ofstream outfile ("test.txt");
    outfile << "Hey, this is a new test file!" << std::endl;
    outfile.close();

    put_new_object(s3_client, bucket1, "asdf", "test.txt");

    auto outcome_list_bucket1 = list_objects(s3_client, bucket1);
    auto outcome_list_bucket2 = list_objects(s3_client, bucket2);

    EXPECT_TRUE(outcome_list_bucket1.IsSuccess());
    EXPECT_TRUE(outcome_list_bucket2.IsSuccess());

    auto objects_bucket1 = outcome_list_bucket1.GetResult().GetContents();
    auto objects_bucket2 = outcome_list_bucket2.GetResult().GetContents();

    EXPECT_EQ(0, objects_bucket2.size());
    EXPECT_EQ(1, objects_bucket1.size());
}

TEST_F(S3Tests, S3Tests_BucketNotExist_Test) {
    Aws::String bucket;
    bucket = get_random_bucket_name();

    auto outcome_list = list_objects(s3_client, bucket);

    EXPECT_FALSE(outcome_list.IsSuccess());
    EXPECT_EQ(Aws::Http::HttpResponseCode::NOT_FOUND, outcome_list.GetError().GetResponseCode());
    EXPECT_EQ(Aws::S3::S3Errors::NO_SUCH_BUCKET, outcome_list.GetError().GetErrorType());
    EXPECT_EQ("NoSuchBucket", outcome_list.GetError().GetExceptionName());
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}