TEMPLATE = subdirs

defineTest(errorMessage) {
    message("Cannot build with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.9.5.")
}

!isEqual(QT_MAJOR_VERSION, 5) {
    errorMessage()
}
else : lessThan(QT_MINOR_VERSION, 5) {
    errorMessage()
} else {

  SUBDIRS = FileReaderServer \
            PlainTextEditWithPartedText \
}
