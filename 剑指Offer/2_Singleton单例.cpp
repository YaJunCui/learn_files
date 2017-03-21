#include <iostream>
#include <string>

using namespace std;

class Singleton                            //饿汉法
{
public:
  static Singleton& get_instance()
  {
    return *instance;
  }

private:
  Singleton()
  {
    static int tmp = 100;
    cout << "I am Singleton, id = " << tmp++ << endl;
  }
  Singleton(const Singleton&);
  Singleton& operator=(const Singleton&);

  static Singleton *instance;
};
Singleton* Singleton::instance = new Singleton;

int main()
{
  Singleton& s1 = Singleton::get_instance();
  Singleton& s2 = Singleton::get_instance();

  return 0;
}