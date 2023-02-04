#ifndef COMMON_H
#define COMMON_H
#include <QRect>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <QString>

#define nameOf( val ) ( #val )
typedef std::vector< std::vector<double> > DotMatrix;
const std::string CONF_FILE_NAME = "conf.txt";
const std::string PERS_CONF_FILE_NAME = "persConf.txt";

inline bool isPointInRectangle(std::pair<int,int> p, QRect r){
	if(p.first <= r.x())
		return false;
	if(p.second <= r.y())
		return false;
	if(p.first > r.x()+r.width())
		return false;
	if(p.second > r.y()+r.height())
		return false;
	return true;
}
inline bool isPointInRectangle(int pX, int pY, QRect r){
	return isPointInRectangle( std::pair<int,int>(pX,pY), r );
}
inline bool isPointInRectangle(int pX, int pY, int rTop, int rLeft, int rW, int rH){
	return isPointInRectangle( std::pair<int,int>(pX,pY), QRect(rTop, rLeft, rW, rH) );
}

template<class Constrained>
struct Iteratable{
	static void constraints(Constrained c){auto it = c.begin() ; it != c.end() ; ++it;}

	Iteratable(){
		void(*unused)(Constrained)=constraints;
		(void)unused;
	}
};
template<class T>
inline std::string str(T iteratable){
	Iteratable<T> i;
	std::string ret;
	for (auto it = iteratable.begin(); it != iteratable.end() ; ++it) {
    std::locale::global(std::locale::classic());
    //https://github.com/nlohmann/json/issues/51
		ret += std::to_string(*it)+", ";
	}
	return ret;
}
template<>
inline std::string str<const char*>(const char* iteratable){
	std::string ret(iteratable);
	return ret;
}
		
template<class T>
inline QString qstr(T iteratable){
	Iteratable<T> i;
	QString ret;
  std::locale::global(std::locale::classic());
  //https://github.com/nlohmann/json/issues/51
	for (auto it = iteratable.begin(); it != iteratable.end() ; ++it) {
		ret += std::to_string(*it)+", ";
	}
	return ret;
}
template<>
inline QString qstr<const char*>(const char* iteratable){
	QString ret(iteratable);
	return ret;
}
#endif // COMMON_H
