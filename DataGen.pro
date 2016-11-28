#-------------------------------------------------
#
# Project created by QtCreator 2015-03-04T16:54:16
#
#-------------------------------------------------

QT       += core gui xml  gui-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DataGen
TEMPLATE = app


SOURCES += src/main.cpp\
        src/dialog.cpp \
    src/xlsx/xlsxabstractooxmlfile.cpp \
    src/xlsx/xlsxabstractsheet.cpp \
    src/xlsx/xlsxcell.cpp \
    src/xlsx/xlsxcellformula.cpp \
    src/xlsx/xlsxcellrange.cpp \
    src/xlsx/xlsxcellreference.cpp \
    src/xlsx/xlsxchart.cpp \
    src/xlsx/xlsxchartsheet.cpp \
    src/xlsx/xlsxcolor.cpp \
    src/xlsx/xlsxconditionalformatting.cpp \
    src/xlsx/xlsxcontenttypes.cpp \
    src/xlsx/xlsxdatavalidation.cpp \
    src/xlsx/xlsxdocpropsapp.cpp \
    src/xlsx/xlsxdocpropscore.cpp \
    src/xlsx/xlsxdocument.cpp \
    src/xlsx/xlsxdrawing.cpp \
    src/xlsx/xlsxdrawinganchor.cpp \
    src/xlsx/xlsxformat.cpp \
    src/xlsx/xlsxmediafile.cpp \
    src/xlsx/xlsxnumformatparser.cpp \
    src/xlsx/xlsxrelationships.cpp \
    src/xlsx/xlsxrichstring.cpp \
    src/xlsx/xlsxsharedstrings.cpp \
    src/xlsx/xlsxsimpleooxmlfile.cpp \
    src/xlsx/xlsxstyles.cpp \
    src/xlsx/xlsxtheme.cpp \
    src/xlsx/xlsxutility.cpp \
    src/xlsx/xlsxworkbook.cpp \
    src/xlsx/xlsxworksheet.cpp \
    src/xlsx/xlsxzipreader.cpp \
    src/xlsx/xlsxzipwriter.cpp \
    src/excelfilemodel.cpp \
    src/procthread.cpp \
    src/exceltableview.cpp \
    src/zexcelparser.cpp \
    src/zconf.cpp \
    src/zutils.cpp

HEADERS  += \
    src/xlsx/xlsxabstractooxmlfile.h \
    src/xlsx/xlsxabstractooxmlfile_p.h \
    src/xlsx/xlsxabstractsheet.h \
    src/xlsx/xlsxabstractsheet_p.h \
    src/xlsx/xlsxcell.h \
    src/xlsx/xlsxcell_p.h \
    src/xlsx/xlsxcellformula.h \
    src/xlsx/xlsxcellformula_p.h \
    src/xlsx/xlsxcellrange.h \
    src/xlsx/xlsxcellreference.h \
    src/xlsx/xlsxchart.h \
    src/xlsx/xlsxchart_p.h \
    src/xlsx/xlsxchartsheet.h \
    src/xlsx/xlsxchartsheet_p.h \
    src/xlsx/xlsxcolor_p.h \
    src/xlsx/xlsxconditionalformatting.h \
    src/xlsx/xlsxconditionalformatting_p.h \
    src/xlsx/xlsxcontenttypes_p.h \
    src/xlsx/xlsxdatavalidation.h \
    src/xlsx/xlsxdatavalidation_p.h \
    src/xlsx/xlsxdocpropsapp_p.h \
    src/xlsx/xlsxdocpropscore_p.h \
    src/xlsx/xlsxdocument.h \
    src/xlsx/xlsxdocument_p.h \
    src/xlsx/xlsxdrawing_p.h \
    src/xlsx/xlsxdrawinganchor_p.h \
    src/xlsx/xlsxformat.h \
    src/xlsx/xlsxformat_p.h \
    src/xlsx/xlsxglobal.h \
    src/xlsx/xlsxmediafile_p.h \
    src/xlsx/xlsxnumformatparser_p.h \
    src/xlsx/xlsxrelationships_p.h \
    src/xlsx/xlsxrichstring.h \
    src/xlsx/xlsxrichstring_p.h \
    src/xlsx/xlsxsharedstrings_p.h \
    src/xlsx/xlsxsimpleooxmlfile_p.h \
    src/xlsx/xlsxstyles_p.h \
    src/xlsx/xlsxtheme_p.h \
    src/xlsx/xlsxutility_p.h \
    src/xlsx/xlsxworkbook.h \
    src/xlsx/xlsxworkbook_p.h \
    src/xlsx/xlsxworksheet.h \
    src/xlsx/xlsxworksheet_p.h \
    src/xlsx/xlsxzipreader_p.h \
    src/xlsx/xlsxzipwriter_p.h \
    src/excelfilemodel.h \
    src/procthread.h \
    src/dialog.h \
    src/exceltableview.h \
    src/zexcelparser.h \
    src/zdumper.h \
    src/zdumperjson.h \
    src/ztypes.h \
    src/zdumperlua.h \
    src/zdumperphp.h \
    zabout.h \
    src/zconf.h \
    src/zutils.h

FORMS    += dialog.ui

unix: CONFIG += link_pkgconfig
