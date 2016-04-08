#include <sstream>
#include <string>

#include "gtest/gtest.h"
#include "config_parser.h"

class NginxStringConfigTest : public ::testing::Test {
 protected:
  bool ParseString(const std::string& config_string) {
    std::stringstream config_stream(config_string);
    return parser_.Parse(&config_stream, &out_config_);
  }
  NginxConfigParser parser_;
  NginxConfig out_config_;
};

TEST_F(NginxStringConfigTest, SimpleConfig) {
  EXPECT_TRUE(ParseString("foo bar;"));
  // We can also add expectations about the contents of the statements:
  EXPECT_EQ(1, out_config_.statements_.size());
  EXPECT_EQ("foo", out_config_.statements_.at(0)->tokens_.at(0));
  EXPECT_EQ("bar", out_config_.statements_.at(0)->tokens_.at(1));
}

TEST_F(NginxStringConfigTest, EmptyConfig) {
  EXPECT_FALSE(ParseString(""));
}

TEST_F(NginxStringConfigTest, BadSemiColon) {
  EXPECT_FALSE(ParseString(";bad semi"));
}

TEST_F(NginxStringConfigTest, BracketConfig) {
  EXPECT_TRUE(ParseString("server { listen   80; }"));
  EXPECT_EQ(1, out_config_.statements_.size());
  EXPECT_EQ("server", out_config_.statements_.at(0)->tokens_.at(0));
  EXPECT_EQ("listen", 
    out_config_.statements_.at(0)->child_block_->statements_.at(0)->tokens_.at(0));
  EXPECT_EQ("80", 
    out_config_.statements_.at(0)->child_block_->statements_.at(0)->tokens_.at(1));
}

TEST_F(NginxStringConfigTest, NestedBracket) {
  EXPECT_TRUE(ParseString("server { listen 80; { test; } }"));
  EXPECT_EQ(1, out_config_.statements_.size());
  EXPECT_EQ("server", out_config_.statements_.at(0)->tokens_.at(0));
  EXPECT_EQ("test",
    out_config_.statements_.at(0)->child_block_->
    statements_.at(0)->child_block_->statements_.at(0)->tokens_.at(0));
  EXPECT_EQ("listen", 
    out_config_.statements_.at(0)->child_block_->statements_.at(0)->tokens_.at(0));
}