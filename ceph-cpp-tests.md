First I did this to build a gtest project with cmake: https://www.youtube.com/watch?v=quC5WSlB-48

Then I did this for aws-sdk to work:

git clone https://github.com/aws/aws-sdk-cpp
cd aws-sdk-cpp
cmake -DBUILD_ONLY="s3" -DCMAKE_BUILD_TYPE=Release -DCPP_STANDARD=17 ./
make
sudo make install

I had these missing deps causing a libcurl.so error:
sudo apt-get install --reinstall libgnutls-openssl27 libcurl4-gnutls-dev

I stole my CMakeLists.txt code for AWS from:
  https://github.com/scanner-research/storehouse/blob/52189a0ed9958c24b8451127ea60dbf1ddbe4237/CMakeLists.tt

How to write tests in GTest: https://www.youtube.com/watch?v=vDpM--9XzF0

Some examples of using S3 from the SDK: https://docs.aws.amazon.com/sdk-for-cpp/v1/developer-guide/examples-s3-buckets.html

