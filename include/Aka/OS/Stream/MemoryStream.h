#pragma once

#include <Aka/OS/Stream/Stream.h>

namespace aka {

class MemoryStream : public Stream
{
public:
	// Read constructor
	MemoryStream(const uint8_t* bytes, size_t size);
	MemoryStream(const std::vector<uint8_t>& bytes);
	// Write constructor
	MemoryStream(uint8_t* bytes, size_t size);
	MemoryStream(std::vector<uint8_t>& bytes);
	~MemoryStream();

	void skim(size_t count) override;
	void seek(size_t position) override;
	size_t size() const override;
	size_t offset() override;
	void rewind() override;

	const uint8_t* data(size_t len) const;
protected:
	void readData(void* data, size_t size) override;
	void writeData(const void* data, size_t size) override;
private:
	const uint8_t* m_bytes;  // Const bytes of the non-owned buffer. Used for reading.
	uint8_t* m_bytesWrite;  // Bytes of the non-owned buffer. Used for writing
	size_t m_size;     // Size of the buffer.
	size_t m_offset;   // Current offset position.
};

};