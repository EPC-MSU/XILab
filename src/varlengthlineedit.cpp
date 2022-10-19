#include "varlengthlineedit.h"

varlengthValidator::varlengthValidator(int bytes)
{
	this->bytes = bytes;
}
varlengthValidator::~varlengthValidator(void)
{
}
void varlengthValidator::setByteLength(int bytes)
{
	this->bytes = bytes;
}

QValidator::State varlengthValidator::validate( QString & input, int & pos ) const
{
	Q_UNUSED(pos)
	if (input.toUtf8().size() <= bytes)
		return QValidator::Acceptable;
	else
		return QValidator::Invalid;
}


varlengthLineEdit::varlengthLineEdit(QWidget *parent):QLineEdit(parent)
{
	v = new varlengthValidator( this->maxLength() );
	QLineEdit::setValidator ( v );
}

varlengthLineEdit::~varlengthLineEdit(void)
{
	delete v;
}

void varlengthLineEdit::setMaxLength(int length)
{
	v->setByteLength( length );
	QLineEdit::setMaxLength( length );
}
