#ifndef MYEXCEPTION_H
#define MYEXCEPTION_H

class my_exception : public std::exception
{
public:
	my_exception(QString text) {
		m_text = text;
	}
	QString text() {
		return m_text;
	}
	virtual ~my_exception() throw() {};
private:
	QString m_text;
};

class critical_exception : public my_exception
{
public:
	critical_exception(QString text) : my_exception(text) {} ;
};
#endif //MYEXCEPTION_H
