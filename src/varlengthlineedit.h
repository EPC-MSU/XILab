#include <QLineEdit>
#include <QValidator>

class varlengthValidator :
	public QValidator
{
public:
	varlengthValidator(int);
	~varlengthValidator(void);
	QValidator::State validate ( QString & input, int & pos ) const;
	void setByteLength (int);
private:
	int bytes;
};

class varlengthLineEdit :
	public QLineEdit
{
public:
	varlengthLineEdit(QWidget *parent = NULL);
	~varlengthLineEdit(void);
	void setMaxLength(int length);
private:
	varlengthValidator *v;
};
