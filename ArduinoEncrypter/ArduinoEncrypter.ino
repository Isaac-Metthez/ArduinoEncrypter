#include <SPI.h>
#include "encryptedCom.hpp"
#include <string.h>

namespace	globals
{    
  communication::EncryptedCom com;
}

void setup()
{
  globals::com.setup();

  globals::com.send("Test if encryption works vdyjbv kjbydvs lkjbydvs bkjlyvkjlyvdkljy vkjys dvl ydvshkljdvsiudsvylkdvsylgyv kjhvxc kjh vyx,jyxh gkjyh kjchvyjxhcg hb yxdvkjhg kyds hgysdkjg fydsjzgyds kjyhfvkydsjg sdv29c8834a7c4f2249d84fde42e95fa246d44fb815f9917db507c40c8ff3f4d7ace10b14424588d6a6973bd5f80f6c1f4e1bfdbb531f5ff24bc4c8226bde1feb4ec1781e4829268c5a025909bcff4c5e2c78ef9997529a127d36baf25ae002925ba67d6e42993300010d308948d00bd9734c4187a1348956b25eb4fe8f8d34kjhygs kjh hjg ydskhjsd fkdgs jv kjb xycvkjvyds jkhv ycxkvsyd kjhgsydvhkvyds kjydsbjydsbjkydsbkjyds");
}
void loop()
{
  globals::com.loop();
}
