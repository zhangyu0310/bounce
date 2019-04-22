// Buffer���gtest�ļ�
// ����Buffer��Ļ�������

#include <bounce/buffer.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>

/*TEST(Buffer, append)
{
	std::string str("cn_LittleYu");
	buffer1.append(str);
	EXPECT_EQ(buffer1.readableBytes(), str.size());

	char strs[] = "poppinzhang";
	buffer2.append(strs, sizeof strs);
	EXPECT_EQ(buffer2.readableBytes(), sizeof strs);
}

TEST(Buffer, readString)
{
	std::string str1 = buffer1.readAsString(buffer1.readableBytes());
	EXPECT_STREQ(str1.c_str(), "cn_LittleYu");

	std::string str2 = buffer2.readAllAsString();
	EXPECT_STREQ(str2.c_str(), "poppinzhang");
}*/

// ������������ַ���
class RandomString {
public:
	RandomString(int num, int len) : num_(num), len_(len) 
	{
		get_rand_string_vector();
	}

	std::vector<std::string> getVector() { return vec_; }
private:
	const std::string STRING_DICTIONARY =
		"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	void get_rand_string_vector() {
		srand((unsigned int)time((time_t*)NULL));
		size_t len_of_dictionary = STRING_DICTIONARY.size();
		for (int i = 0; i < num_; ++i) {
			std::string tmp;
			for (int j = 0; j < len_ - 1; ++j) {
				tmp.push_back(STRING_DICTIONARY[rand() % len_of_dictionary]);
			}
			tmp.push_back('\0');
			vec_.push_back(tmp);
		}
	}

	int num_;
	int len_;
	std::vector<std::string> vec_;
};

class RandomStringTest : public testing::TestWithParam<std::string> {
protected:
	void setUp() {
		str = GetParam();
	}
	void TearDown() {

	}

	std::string str;
};

RandomString rstr(3, 15);

INSTANTIATE_TEST_CASE_P(RandomString,
	RandomStringTest,
	testing::ValuesIn(rstr.getVector()));


TEST_P(RandomStringTest, appendAndRead)
{
	bounce::Buffer buffer1;
	buffer1.append(str);
	EXPECT_EQ(buffer1.readableBytes(), str.size());
	std::string str1 = buffer1.readAsString(buffer1.readableBytes());
	EXPECT_STREQ(str1.c_str(), str.c_str());

	bounce::Buffer buffer2;
	buffer2.append(str.c_str(), str.size());
	EXPECT_EQ(buffer2.readableBytes(), str.size());
	std::string str2 = buffer2.readAllAsString();
	EXPECT_STREQ(str2.c_str(), str.c_str());
}

TEST(Buffer, append)
{
    bounce::Buffer buffer1;
    buffer1.writeInt8(11);
    buffer1.writeInt16(22);
    buffer1.writeInt32(33);
	EXPECT_EQ(buffer1.readInt8(), 11);
	EXPECT_EQ(buffer1.readInt16(), 22);
	EXPECT_EQ(buffer1.readInt32(), 33);
}