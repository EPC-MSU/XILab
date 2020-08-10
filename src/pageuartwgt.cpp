#include <pageuartwgt.h>
#include <ui_pageuartwgt.h>

PageUartWgt::PageUartWgt(QWidget *parent, MotorSettings *motorStgs) :
    QWidget(parent),
    m_ui(new Ui::PageUartWgtClass)
{
	m_ui->setupUi(this);
	mStgs = motorStgs;
}

PageUartWgt::~PageUartWgt()
{
    delete m_ui;
}

void PageUartWgt::FromClassToUi()
{
	int index = m_ui->speedBox->findText(toStr(mStgs->uart.Speed));
	if (index != -1)
		m_ui->speedBox->setCurrentIndex(index);
	else
		m_ui->speedBox->setCurrentIndex(0);
	m_ui->checkBox->setChecked(mStgs->uart.UARTSetupFlags & UART_PARITY_BIT_USE);
	m_ui->parityBox->setCurrentIndex(mStgs->uart.UARTSetupFlags & UART_PARITY_BITS);
	if (mStgs->uart.UARTSetupFlags & UART_STOP_BIT)
		m_ui->stopBox->setCurrentIndex(0);
	else
		m_ui->stopBox->setCurrentIndex(1);

	m_ui->label->setEnabled(m_ui->checkBox->isChecked());
	m_ui->parityBox->setEnabled(m_ui->checkBox->isChecked());
}

void PageUartWgt::FromUiToClass(MotorSettings *lmStgs)
{
	lmStgs->uart.UARTSetupFlags = 0;

	lmStgs->uart.Speed = m_ui->speedBox->itemText(m_ui->speedBox->currentIndex()).toInt();
	setUnsetBit(m_ui->checkBox->isChecked(), &lmStgs->uart.UARTSetupFlags, UART_PARITY_BIT_USE);
	
	lmStgs->uart.UARTSetupFlags &= ~UART_PARITY_BITS;
	switch (m_ui->parityBox->currentIndex()) {
		case 0: lmStgs->uart.UARTSetupFlags |= UART_PARITY_BIT_EVEN; break;
		case 1: lmStgs->uart.UARTSetupFlags |= UART_PARITY_BIT_ODD; break;
		case 2: lmStgs->uart.UARTSetupFlags |= UART_PARITY_BIT_SPACE; break;
		case 3: lmStgs->uart.UARTSetupFlags |= UART_PARITY_BIT_MARK; break;
		default: break;
	}

	setUnsetBit((m_ui->stopBox->currentIndex() == 0), &lmStgs->uart.UARTSetupFlags, UART_STOP_BIT);
}
