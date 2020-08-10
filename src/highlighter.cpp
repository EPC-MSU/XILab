#include <QtGui>
#include "highlighter.h"

namespace Highlight {
	typedef enum {
		NORMAL = 1,
		MULTI_LINE_COMMENT = 3
	} state;
}

//! [0]
Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                    << "\\bvoid\\b" << "\\bvolatile\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
//! [0] //! [1]
    }
//! [1]

//! [2]
	classFormat.setFontWeight(QFont::Normal);
    classFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\\b[0-9.]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    classFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegExp("-\\b[0-9.]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);
//! [2]

//! [3]
    multiLineCommentFormat.setForeground(Qt::gray);
//! [3]

//! [4]

//! [4]

//! [5]
    functionFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);
//! [5]
    classFormat.setForeground(Qt::blue);
	#include "qtscript-highlights.c"
    rule.format = classFormat;
    highlightingRules.append(rule);
//! [6]
    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");

    singleLineCommentExpression = QRegExp("//[^\n]*");
    singleLineCommentFormat.setForeground(Qt::gray);
}
//! [6]

//! [7]
void Highlighter::highlightBlock(const QString &text)
{
	if (!text.isEmpty()) {
		foreach (const HighlightingRule &rule, highlightingRules) {
			QRegExp expression(rule.pattern);
			int index = expression.indexIn(text);
			while (index >= 0) {
				int length = expression.matchedLength();
				setFormat(index, length, rule.format);
				index = expression.indexIn(text, index + length);
			}
		}
	}
//! [7] //! [8]
    setCurrentBlockState(Highlight::NORMAL);
//! [8]

//! [9]
    int startIndex = 0;
    int singleStartIndex = singleLineCommentExpression.indexIn(text);
    if (previousBlockState() != Highlight::MULTI_LINE_COMMENT)
        startIndex = commentStartExpression.indexIn(text);
    if (singleStartIndex >= 0 && startIndex < 0) {
        setFormat(singleStartIndex, text.length() - singleStartIndex, singleLineCommentFormat);
    }

//! [9] //! [10]
    while (startIndex >= 0) {
//! [10] //! [11]
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (singleStartIndex >= 0 && startIndex > singleStartIndex) {
            setCurrentBlockState(Highlight::NORMAL);
            commentLength = text.length() - singleStartIndex;
            setFormat(singleStartIndex, commentLength, singleLineCommentFormat);
            break;
        }
        if (endIndex == -1) {
            setCurrentBlockState(Highlight::MULTI_LINE_COMMENT);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        singleStartIndex = singleLineCommentExpression.indexIn(text, startIndex + commentLength);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }

}
//! [11]
