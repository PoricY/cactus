#include "cactusGlobalsPrivate.h"

////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
//Meta sequence functions.
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////

MetaSequence *metaSequence_construct2(Name name, int32_t start,
		int32_t length, int64_t fileOffset, const char *header,
		Name eventName, NetDisk *netDisk) {
	MetaSequence *metaSequence;

	metaSequence = st_malloc(sizeof(MetaSequence));
	metaSequence->name = name;
	assert(length >= 0);
	metaSequence->start = start;
	metaSequence->length = length;
	metaSequence->fileOffset = fileOffset;
	metaSequence->eventName = eventName;
	metaSequence->netDisk = netDisk;
	metaSequence->header = st_string_copy(header != NULL ? header : "");

	netDisk_addMetaSequence(netDisk, metaSequence);
	return metaSequence;
}

MetaSequence *metaSequence_construct(int32_t start, int32_t length,
		const char *string, const char *header, Name eventName, NetDisk *netDisk) {
	int64_t fileOffset;
	fileOffset = netDisk_addString(netDisk, string, length);
	return metaSequence_construct2(netDisk_getUniqueID(netDisk), start, length,
			fileOffset, header, eventName, netDisk);
}

void metaSequence_destruct(MetaSequence *metaSequence) {
	netDisk_unloadMetaSequence(metaSequence->netDisk, metaSequence);
	free(metaSequence->header);
	free(metaSequence);
}

Name metaSequence_getName(MetaSequence *metaSequence) {
	return metaSequence->name;
}

int32_t metaSequence_getStart(MetaSequence *metaSequence) {
	return metaSequence->start;
}

int32_t metaSequence_getLength(MetaSequence *metaSequence) {
	return metaSequence->length;
}

Name metaSequence_getEventName(MetaSequence *metaSequence) {
	return metaSequence->eventName;
}

char *metaSequence_getString(MetaSequence *metaSequence, int32_t start, int32_t length, int32_t strand) {
	assert(start >= metaSequence_getStart(metaSequence));
	assert(length >= 0);
	assert(start + length <= metaSequence_getStart(metaSequence) + metaSequence_getLength(metaSequence));
	return netDisk_getString(metaSequence->netDisk, metaSequence->fileOffset, start - metaSequence_getStart(metaSequence), length, strand);
}

const char *metaSequence_getHeader(MetaSequence *metaSequence) {
	return metaSequence->header;
}

/*
 * Private functions
 */

int64_t metaSequence_getFileOffset(MetaSequence *metaSequence) {
	return metaSequence->fileOffset;
}

/*
 * Serialisation functions.
 */

void metaSequence_writeBinaryRepresentation(MetaSequence *metaSequence,
		void (*writeFn)(const void * ptr, size_t size, size_t count)) {
	binaryRepresentation_writeElementType(CODE_META_SEQUENCE, writeFn);
	binaryRepresentation_writeName(metaSequence_getName(metaSequence), writeFn);
	binaryRepresentation_writeInteger(metaSequence_getStart(metaSequence), writeFn);
	binaryRepresentation_writeInteger(metaSequence_getLength(metaSequence), writeFn);
	binaryRepresentation_writeName(metaSequence_getEventName(metaSequence), writeFn);
	binaryRepresentation_write64BitInteger(metaSequence_getFileOffset(metaSequence), writeFn);
	binaryRepresentation_writeString(metaSequence_getHeader(metaSequence), writeFn);
}

MetaSequence *metaSequence_loadFromBinaryRepresentation(void **binaryString,
		NetDisk *netDisk) {
	MetaSequence *metaSequence;
	Name name;
	int32_t start;
	int32_t length;
	int64_t fileOffset;
	Name eventName;
	char *header;

	metaSequence = NULL;
	if(binaryRepresentation_peekNextElementType(*binaryString) == CODE_META_SEQUENCE) {
		binaryRepresentation_popNextElementType(binaryString);
		name = binaryRepresentation_getName(binaryString);
		start = binaryRepresentation_getInteger(binaryString);
		length = binaryRepresentation_getInteger(binaryString);
		eventName = binaryRepresentation_getName(binaryString);
		fileOffset = binaryRepresentation_get64BitInteger(binaryString);
		header = binaryRepresentation_getString(binaryString);
		metaSequence = metaSequence_construct2(name, start, length,
				fileOffset, header, eventName, netDisk);
		free(header);
	}
	return metaSequence;
}

