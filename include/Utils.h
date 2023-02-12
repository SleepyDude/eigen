#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>

std::vector<std::string> split(const std::string& s, char delimiter);

template < class ContainerT >
void tokenize(const std::string& str, ContainerT& tokens,
              const std::string& delimiters = " ", bool trimEmpty = false)
{
   std::string::size_type pos, lastPos = 0, length = str.length();

   using value_type = typename ContainerT::value_type;
   using size_type  = typename ContainerT::size_type;

   while(lastPos < length + 1)
   {
      pos = str.find_first_of(delimiters, lastPos);
      if(pos == std::string::npos)
      {
         pos = length;
      }

      if(pos != lastPos || !trimEmpty)
         tokens.push_back(value_type(str.data()+lastPos,
               (size_type)pos-lastPos ));

      lastPos = pos + 1;
   }
}

#endif
