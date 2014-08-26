#include <cstring>
#include <string>
#include <assert.h>

char *binary_replace(char *binString,  int _strlen, int binDataStart, int binDataEnd, char* replaceWith)
{
	char *buffer = (char*)malloc( (strlen(replaceWith)+(_strlen - (binDataEnd-binDataStart)))*sizeof(char) );

	memcpy(buffer, binString, binDataStart);

	strcat(buffer, replaceWith);

	memmove(buffer+binDataStart+strlen(replaceWith), binString+binDataEnd, _strlen - binDataEnd);

	free(binString);

	return buffer;
}

std::string binary_replace(
    std::string const& bin, unsigned bin_start, unsigned bin_end,
    std::string const& replace_with
) {
    return bin.substr(0, bin_start) + replace_with + bin.substr(bin_end);
}
