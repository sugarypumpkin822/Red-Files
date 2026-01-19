#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <functional>

namespace RFUtils {

// Hash algorithm types
enum class HashAlgorithm {
    CRC32,
    CRC64,
    MD5,
    SHA1,
    SHA224,
    SHA256,
    SHA384,
    SHA512,
    SHA3_224,
    SHA3_256,
    SHA3_384,
    SHA3_512,
    BLAKE2b,
    BLAKE2s,
    MurmurHash2,
    MurmurHash3,
    FNV1a,
    FNV1,
    DJB2,
    SDBM,
    CityHash,
    FarmHash,
    xxHash,
    SpookyHash,
    MetroHash,
    HighwayHash,
    Tiger,
    Whirlpool,
    RIPEMD160,
    GOST,
    Panama,
    RadioGatun,
    Skein,
    Keccak,
    SHAKE128,
    SHAKE256,
    BLAKE3,
    SM3,
    Streebog256,
    Streebog512
};

// Hash function base class
class HashFunction {
public:
    HashFunction() = default;
    virtual ~HashFunction() = default;
    
    // Core operations
    virtual void reset() = 0;
    virtual void update(const uint8_t* data, size_t length) = 0;
    virtual void update(const std::string& data) = 0;
    virtual void update(const std::vector<uint8_t>& data) = 0;
    
    // Finalization
    virtual std::vector<uint8_t> finalize() = 0;
    virtual std::string finalizeHex() = 0;
    virtual std::string finalizeBase64() = 0;
    
    // Convenience methods
    virtual std::vector<uint8_t> hash(const uint8_t* data, size_t length) = 0;
    virtual std::vector<uint8_t> hash(const std::string& data) = 0;
    virtual std::vector<uint8_t> hash(const std::vector<uint8_t>& data) = 0;
    
    virtual std::string hashHex(const uint8_t* data, size_t length) = 0;
    virtual std::string hashHex(const std::string& data) = 0;
    virtual std::string hashHex(const std::vector<uint8_t>& data) = 0;
    
    // Properties
    virtual size_t getHashSize() const = 0;
    virtual size_t getBlockSize() const = 0;
    virtual HashAlgorithm getAlgorithm() const = 0;
    virtual std::string getName() const = 0;
    
    // State management
    virtual std::vector<uint8_t> getState() const = 0;
    virtual void setState(const std::vector<uint8_t>& state) = 0;
    virtual void saveState(const std::string& filename) const = 0;
    virtual void loadState(const std::string& filename) = 0;
    
    // Comparison
    virtual bool operator==(const HashFunction& other) const = 0;
    virtual bool operator!=(const HashFunction& other) const = 0;
};

// CRC32 hash function
class CRC32Hash : public HashFunction {
public:
    CRC32Hash(uint32_t polynomial = 0xEDB88320u, uint32_t initialValue = 0xFFFFFFFFu);
    
    void reset() override;
    void update(const uint8_t* data, size_t length) override;
    void update(const std::string& data) override;
    void update(const std::vector<uint8_t>& data) override;
    
    std::vector<uint8_t> finalize() override;
    std::string finalizeHex() override;
    std::string finalizeBase64() override;
    
    std::vector<uint8_t> hash(const uint8_t* data, size_t length) override;
    std::vector<uint8_t> hash(const std::string& data) override;
    std::vector<uint8_t> hash(const std::vector<uint8_t>& data) override;
    
    std::string hashHex(const uint8_t* data, size_t length) override;
    std::string hashHex(const std::string& data) override;
    std::string hashHex(const std::vector<uint8_t>& data) override;
    
    size_t getHashSize() const override;
    size_t getBlockSize() const override;
    HashAlgorithm getAlgorithm() const override;
    std::string getName() const override;
    
    std::vector<uint8_t> getState() const override;
    void setState(const std::vector<uint8_t>& state) override;
    void saveState(const std::string& filename) const override;
    void loadState(const std::string& filename) override;
    
    bool operator==(const HashFunction& other) const override;
    bool operator!=(const HashFunction& other) const override;
    
    // CRC32-specific methods
    void setPolynomial(uint32_t polynomial);
    void setInitialValue(uint32_t initialValue);
    void setFinalXOR(uint32_t finalXOR);
    uint32_t getPolynomial() const { return polynomial_; }
    uint32_t getInitialValue() const { return initialValue_; }
    uint32_t getFinalXOR() const { return finalXOR_; }
    uint32_t getCurrentValue() const { return currentValue_; }
    
private:
    uint32_t polynomial_;
    uint32_t initialValue_;
    uint32_t finalXOR_;
    uint32_t currentValue_;
    std::array<uint32_t, 256> table_;
    
    void generateTable();
    uint32_t processByte(uint8_t byte) const;
};

// MD5 hash function
class MD5Hash : public HashFunction {
public:
    MD5Hash();
    
    void reset() override;
    void update(const uint8_t* data, size_t length) override;
    void update(const std::string& data) override;
    void update(const std::vector<uint8_t>& data) override;
    
    std::vector<uint8_t> finalize() override;
    std::string finalizeHex() override;
    std::string finalizeBase64() override;
    
    std::vector<uint8_t> hash(const uint8_t* data, size_t length) override;
    std::vector<uint8_t> hash(const std::string& data) override;
    std::vector<uint8_t> hash(const std::vector<uint8_t>& data) override;
    
    std::string hashHex(const uint8_t* data, size_t length) override;
    std::string hashHex(const std::string& data) override;
    std::string hashHex(const std::vector<uint8_t>& data) override;
    
    size_t getHashSize() const override;
    size_t getBlockSize() const override;
    HashAlgorithm getAlgorithm() const override;
    std::string getName() const override;
    
    std::vector<uint8_t> getState() const override;
    void setState(const std::vector<uint8_t>& state) override;
    void saveState(const std::string& filename) const override;
    void loadState(const std::string& filename) override;
    
    bool operator==(const HashFunction& other) const override;
    bool operator!=(const HashFunction& other) const override;
    
private:
    std::array<uint32_t, 4> state_;
    std::array<uint64_t, 2> bitCount_;
    std::array<uint8_t, 64> buffer_;
    bool finalized_;
    
    void transform(const uint8_t* block);
    static uint32_t F(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t G(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t H(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t I(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t rotateLeft(uint32_t x, int n);
};

// SHA256 hash function
class SHA256Hash : public HashFunction {
public:
    SHA256Hash();
    
    void reset() override;
    void update(const uint8_t* data, size_t length) override;
    void update(const std::string& data) override;
    void update(const std::vector<uint8_t>& data) override;
    
    std::vector<uint8_t> finalize() override;
    std::string finalizeHex() override;
    std::string finalizeBase64() override;
    
    std::vector<uint8_t> hash(const uint8_t* data, size_t length) override;
    std::vector<uint8_t> hash(const std::string& data) override;
    std::vector<uint8_t> hash(const std::vector<uint8_t>& data) override;
    
    std::string hashHex(const uint8_t* data, size_t length) override;
    std::string hashHex(const std::string& data) override;
    std::string hashHex(const std::vector<uint8_t>& data) override;
    
    size_t getHashSize() const override;
    size_t getBlockSize() const override;
    HashAlgorithm getAlgorithm() const override;
    std::string getName() const override;
    
    std::vector<uint8_t> getState() const override;
    void setState(const std::vector<uint8_t>& state) override;
    void saveState(const std::string& filename) const override;
    void loadState(const std::string& filename) override;
    
    bool operator==(const HashFunction& other) const override;
    bool operator!=(const HashFunction& other) const override;
    
private:
    std::array<uint32_t, 8> state_;
    std::array<uint64_t, 2> bitCount_;
    std::array<uint8_t, 64> buffer_;
    bool finalized_;
    
    void transform(const uint8_t* block);
    static uint32_t rotateRight(uint32_t x, int n);
    static uint32_t ch(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t maj(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t sigma0(uint32_t x);
    static uint32_t sigma1(uint32_t x);
    static uint32_t bigSigma0(uint32_t x);
    static uint32_t bigSigma1(uint32_t x);
};

// MurmurHash3 (128-bit) hash function
class MurmurHash3 : public HashFunction {
public:
    explicit MurmurHash3(uint32_t seed = 0);
    
    void reset() override;
    void update(const uint8_t* data, size_t length) override;
    void update(const std::string& data) override;
    void update(const std::vector<uint8_t>& data) override;
    
    std::vector<uint8_t> finalize() override;
    std::string finalizeHex() override;
    std::string finalizeBase64() override;
    
    std::vector<uint8_t> hash(const uint8_t* data, size_t length) override;
    std::vector<uint8_t> hash(const std::string& data) override;
    std::vector<uint8_t> hash(const std::vector<uint8_t>& data) override;
    
    std::string hashHex(const uint8_t* data, size_t length) override;
    std::string hashHex(const std::string& data) override;
    std::string hashHex(const std::vector<uint8_t>& data) override;
    
    size_t getHashSize() const override;
    size_t getBlockSize() const override;
    HashAlgorithm getAlgorithm() const override;
    std::string getName() const override;
    
    std::vector<uint8_t> getState() const override;
    void setState(const std::vector<uint8_t>& state) override;
    void saveState(const std::string& filename) const override;
    void loadState(const std::string& filename) override;
    
    bool operator==(const HashFunction& other) const override;
    bool operator!=(const HashFunction& other) const override;
    
    // MurmurHash3-specific methods
    void setSeed(uint32_t seed);
    uint32_t getSeed() const { return seed_; }
    
private:
    uint32_t seed_;
    std::array<uint64_t, 4> h_;
    std::array<uint8_t, 16> buffer_;
    size_t length_;
    size_t totalLength_;
    
    static uint64_t rotateLeft(uint64_t x, int r);
    static uint64_t fmix64(uint64_t k);
    void processBlock(const uint64_t* block, size_t blocks);
};

// xxHash (64-bit) hash function
class xxHash : public HashFunction {
public:
    explicit xxHash(uint64_t seed = 0);
    
    void reset() override;
    void update(const uint8_t* data, size_t length) override;
    void update(const std::string& data) override;
    void update(const std::vector<uint8_t>& data) override;
    
    std::vector<uint8_t> finalize() override;
    std::string finalizeHex() override;
    std::string finalizeBase64() override;
    
    std::vector<uint8_t> hash(const uint8_t* data, size_t length) override;
    std::vector<uint8_t> hash(const std::string& data) override;
    std::vector<uint8_t> hash(const std::vector<uint8_t>& data) override;
    
    std::string hashHex(const uint8_t* data, size_t length) override;
    std::string hashHex(const std::string& data) override;
    std::string hashHex(const std::vector<uint8_t>& data) override;
    
    size_t getHashSize() const override;
    size_t getBlockSize() const override;
    HashAlgorithm getAlgorithm() const override;
    std::string getName() const override;
    
    std::vector<uint8_t> getState() const override;
    void setState(const std::vector<uint8_t>& state) override;
    void saveState(const std::string& filename) const override;
    void loadState(const std::string& filename) override;
    
    bool operator==(const HashFunction& other) const override;
    bool operator!=(const HashFunction& other) const override;
    
    // xxHash-specific methods
    void setSeed(uint64_t seed);
    uint64_t getSeed() const { return seed_; }
    
private:
    uint64_t seed_;
    std::array<uint64_t, 4> acc_;
    std::array<uint8_t, 32> buffer_;
    size_t bufferSize_;
    uint64_t totalLength_;
    
    static uint64_t rotateRight(uint64_t x, int r);
    static uint64_t multiply64(uint64_t a, uint64_t b);
    void accumulate();
};

// Hash function factory
class HashFactory {
public:
    static std::unique_ptr<HashFunction> create(HashAlgorithm algorithm);
    static std::unique_ptr<HashFunction> createCRC32(uint32_t polynomial = 0xEDB88320u);
    static std::unique_ptr<HashFunction> createMD5();
    static std::unique_ptr<HashFunction> createSHA1();
    static std::unique_ptr<HashFunction> createSHA224();
    static std::unique_ptr<HashFunction> createSHA256();
    static std::unique_ptr<HashFunction> createSHA384();
    static std::unique_ptr<HashFunction> createSHA512();
    static std::unique_ptr<HashFunction> createSHA3_224();
    static std::unique_ptr<HashFunction> createSHA3_256();
    static std::unique_ptr<HashFunction> createSHA3_384();
    static std::unique_ptr<HashFunction> createSHA3_512();
    static std::unique_ptr<HashFunction> createBLAKE2b();
    static std::unique_ptr<HashFunction> createBLAKE2s();
    static std::unique_ptr<HashFunction> createMurmurHash2(uint32_t seed = 0);
    static std::unique_ptr<HashFunction> createMurmurHash3(uint32_t seed = 0);
    static std::unique_ptr<HashFunction> createFNV1a();
    static std::unique_ptr<HashFunction> createFNV1();
    static std::unique_ptr<HashFunction> createDJB2();
    static std::unique_ptr<HashFunction> createSDBM();
    static std::unique_ptr<HashFunction> createCityHash();
    static std::unique_ptr<HashFunction> createFarmHash();
    static std::unique_ptr<HashFunction> createXXHash(uint64_t seed = 0);
    static std::unique_ptr<HashFunction> createSpookyHash();
    static std::unique_ptr<HashFunction> createMetroHash();
    static std::unique_ptr<HashFunction> createHighwayHash();
    static std::unique_ptr<HashFunction> createTiger();
    static std::unique_ptr<HashFunction> createWhirlpool();
    static std::unique_ptr<HashFunction> createRIPEMD160();
    static std::unique_ptr<HashFunction> createGOST();
    static std::unique_ptr<HashFunction> createPanama();
    static std::unique_ptr<HashFunction> createRadioGatun();
    static std::unique_ptr<HashFunction> createSkein();
    static std::unique_ptr<HashFunction> createKeccak();
    static std::unique_ptr<HashFunction> createSHAKE128();
    static std::unique_ptr<HashFunction> createSHAKE256();
    static std::unique_ptr<HashFunction> createBLAKE3();
    static std::unique_ptr<HashFunction> createSM3();
    static std::unique_ptr<HashFunction> createStreebog256();
    static std::unique_ptr<HashFunction> createStreebog512();
    
    static std::vector<std::string> getAvailableAlgorithms();
    static HashAlgorithm getAlgorithmFromName(const std::string& name);
    static std::string getNameFromAlgorithm(HashAlgorithm algorithm);
    
    // Default hash functions
    static std::unique_ptr<HashFunction> createDefault();
    static std::unique_ptr<HashFunction> createFast();
    static std::unique_ptr<HashFunction> createSecure();
    static std::unique_ptr<HashFunction> createHighQuality();
};

// Hash utilities namespace
namespace HashUtils {
    // Simple hash functions
    uint32_t hash32(const uint8_t* data, size_t length);
    uint32_t hash32(const std::string& data);
    uint32_t hash32(const std::vector<uint8_t>& data);
    
    uint64_t hash64(const uint8_t* data, size_t length);
    uint64_t hash64(const std::string& data);
    uint64_t hash64(const std::vector<uint8_t>& data);
    
    // CRC functions
    uint32_t crc32(const uint8_t* data, size_t length, uint32_t polynomial = 0xEDB88320u, uint32_t initialValue = 0xFFFFFFFFu);
    uint32_t crc32(const std::string& data, uint32_t polynomial = 0xEDB88320u, uint32_t initialValue = 0xFFFFFFFFu);
    uint32_t crc32(const std::vector<uint8_t>& data, uint32_t polynomial = 0xEDB88320u, uint32_t initialValue = 0xFFFFFFFFu);
    
    uint64_t crc64(const uint8_t* data, size_t length, uint64_t polynomial = 0xC96C5795D7870F42ull, uint64_t initialValue = 0xFFFFFFFFFFFFFFFFull);
    uint64_t crc64(const std::string& data, uint64_t polynomial = 0xC96C5795D7870F42ull, uint64_t initialValue = 0xFFFFFFFFFFFFFFFFull);
    uint64_t crc64(const std::vector<uint8_t>& data, uint64_t polynomial = 0xC96C5795D7870F42ull, uint64_t initialValue = 0xFFFFFFFFFFFFFFFFull);
    
    // Cryptographic hash functions
    std::vector<uint8_t> md5(const uint8_t* data, size_t length);
    std::vector<uint8_t> md5(const std::string& data);
    std::vector<uint8_t> md5(const std::vector<uint8_t>& data);
    
    std::string md5Hex(const uint8_t* data, size_t length);
    std::string md5Hex(const std::string& data);
    std::string md5Hex(const std::vector<uint8_t>& data);
    
    std::vector<uint8_t> sha1(const uint8_t* data, size_t length);
    std::vector<uint8_t> sha1(const std::string& data);
    std::vector<uint8_t> sha1(const std::vector<uint8_t>& data);
    
    std::string sha1Hex(const uint8_t* data, size_t length);
    std::string sha1Hex(const std::string& data);
    std::string sha1Hex(const std::vector<uint8_t>& data);
    
    std::vector<uint8_t> sha256(const uint8_t* data, size_t length);
    std::vector<uint8_t> sha256(const std::string& data);
    std::vector<uint8_t> sha256(const std::vector<uint8_t>& data);
    
    std::string sha256Hex(const uint8_t* data, size_t length);
    std::string sha256Hex(const std::string& data);
    std::string sha256Hex(const std::vector<uint8_t>& data);
    
    std::vector<uint8_t> sha512(const uint8_t* data, size_t length);
    std::vector<uint8_t> sha512(const std::string& data);
    std::vector<uint8_t> sha512(const std::vector<uint8_t>& data);
    
    std::string sha512Hex(const uint8_t* data, size_t length);
    std::string sha512Hex(const std::string& data);
    std::string sha512Hex(const std::vector<uint8_t>& data);
    
    // Non-cryptographic hash functions
    uint32_t fnv1a32(const uint8_t* data, size_t length, uint32_t seed = 2166136261u);
    uint32_t fnv1a32(const std::string& data, uint32_t seed = 2166136261u);
    uint32_t fnv1a32(const std::vector<uint8_t>& data, uint32_t seed = 2166136261u);
    
    uint64_t fnv1a64(const uint8_t* data, size_t length, uint64_t seed = 14695981039346656037ull);
    uint64_t fnv1a64(const std::string& data, uint64_t seed = 14695981039346656037ull);
    uint64_t fnv1a64(const std::vector<uint8_t>& data, uint64_t seed = 14695981039346656037ull);
    
    uint32_t djb2(const uint8_t* data, size_t length, uint32_t seed = 5381u);
    uint32_t djb2(const std::string& data, uint32_t seed = 5381u);
    uint32_t djb2(const std::vector<uint8_t>& data, uint32_t seed = 5381u);
    
    uint32_t sdbm(const uint8_t* data, size_t length, uint32_t seed = 0u);
    uint32_t sdbm(const std::string& data, uint32_t seed = 0u);
    uint32_t sdbm(const std::vector<uint8_t>& data, uint32_t seed = 0u);
    
    uint64_t murmurHash2_64(const uint8_t* data, size_t length, uint64_t seed = 0);
    uint64_t murmurHash2_64(const std::string& data, uint64_t seed = 0);
    uint64_t murmurHash2_64(const std::vector<uint8_t>& data, uint64_t seed = 0);
    
    std::array<uint64_t, 2> murmurHash3_128(const uint8_t* data, size_t length, uint32_t seed = 0);
    std::array<uint64_t, 2> murmurHash3_128(const std::string& data, uint32_t seed = 0);
    std::array<uint64_t, 2> murmurHash3_128(const std::vector<uint8_t>& data, uint32_t seed = 0);
    
    uint64_t xxHash64(const uint8_t* data, size_t length, uint64_t seed = 0);
    uint64_t xxHash64(const std::string& data, uint64_t seed = 0);
    uint64_t xxHash64(const std::vector<uint8_t>& data, uint64_t seed = 0);
    
    // Hash comparison
    bool compareHashes(const std::vector<uint8_t>& hash1, const std::vector<uint8_t>& hash2);
    bool compareHashes(const std::string& hash1, const std::string& hash2);
    
    // Hash verification
    bool verifyHash(const uint8_t* data, size_t length, const std::vector<uint8_t>& expectedHash, HashAlgorithm algorithm);
    bool verifyHash(const std::string& data, const std::vector<uint8_t>& expectedHash, HashAlgorithm algorithm);
    bool verifyHash(const std::vector<uint8_t>& data, const std::vector<uint8_t>& expectedHash, HashAlgorithm algorithm);
    
    bool verifyHashHex(const uint8_t* data, size_t length, const std::string& expectedHash, HashAlgorithm algorithm);
    bool verifyHashHex(const std::string& data, const std::string& expectedHash, HashAlgorithm algorithm);
    bool verifyHashHex(const std::vector<uint8_t>& data, const std::string& expectedHash, HashAlgorithm algorithm);
    
    // Hash utilities
    std::string hashToString(const std::vector<uint8_t>& hash);
    std::vector<uint8_t> hashFromString(const std::string& hash);
    
    std::string hashToBase64(const std::vector<uint8_t>& hash);
    std::vector<uint8_t> hashFromBase64(const std::string& hash);
    
    // Hash analysis
    size_t getHashSize(HashAlgorithm algorithm);
    size_t getBlockSize(HashAlgorithm algorithm);
    bool isCryptographic(HashAlgorithm algorithm);
    bool isCollisionResistant(HashAlgorithm algorithm);
    bool isPreimageResistant(HashAlgorithm algorithm);
    bool isSecondPreimageResistant(HashAlgorithm algorithm);
    
    // Hash performance
    struct HashPerformance {
        HashAlgorithm algorithm;
        std::string name;
        double throughput; // MB/s
        double latency; // ms
        size_t hashSize;
        size_t blockSize;
        bool isCryptographic;
    };
    
    std::vector<HashPerformance> benchmarkHashes(const std::vector<uint8_t>& testData);
    HashPerformance benchmarkHash(HashAlgorithm algorithm, const std::vector<uint8_t>& testData);
    
    // Hash collision testing
    struct CollisionTestResult {
        HashAlgorithm algorithm;
        size_t samples;
        size_t collisions;
        double collisionRate;
        double expectedCollisionRate;
        bool passed;
    };
    
    CollisionTestResult testCollisions(HashAlgorithm algorithm, size_t samples = 1000000);
    std::vector<CollisionTestResult> testAllCollisions(size_t samples = 1000000);
    
    // Hash distribution testing
    struct DistributionTestResult {
        HashAlgorithm algorithm;
        size_t samples;
        double chiSquared;
        double pValue;
        bool isUniform;
        bool passed;
    };
    
    DistributionTestResult testDistribution(HashAlgorithm algorithm, size_t samples = 1000000);
    std::vector<DistributionTestResult> testAllDistributions(size_t samples = 1000000);
    
    // Hash avalanche testing
    struct AvalancheTestResult {
        HashAlgorithm algorithm;
        size_t samples;
        double averageChangedBits;
        double expectedChangedBits;
        double variance;
        bool passed;
    };
    
    AvalancheTestResult testAvalanche(HashAlgorithm algorithm, size_t samples = 1000000);
    std::vector<AvalancheTestResult> testAllAvalanche(size_t samples = 1000000);
    
    // Hash combination
    std::vector<uint8_t> combineHashes(const std::vector<std::vector<uint8_t>>& hashes);
    std::vector<uint8_t> combineHashesXOR(const std::vector<std::vector<uint8_t>>& hashes);
    std::vector<uint8_t> combineHashesConcat(const std::vector<std::vector<uint8_t>>& hashes);
    std::vector<uint8_t> combineHashesHash(const std::vector<std::vector<uint8_t>>& hashes, HashAlgorithm algorithm = HashAlgorithm::SHA256);
    
    // Hash-based data structures
    template<typename T>
    class HashSet {
    public:
        explicit HashSet(HashAlgorithm algorithm = HashAlgorithm::SHA256);
        
        void insert(const T& item);
        bool contains(const T& item) const;
        bool remove(const T& item);
        void clear();
        
        size_t size() const;
        bool empty() const;
        
        std::vector<T> getItems() const;
        
    private:
        std::unique_ptr<HashFunction> hashFunction_;
        std::unordered_set<std::vector<uint8_t>> hashSet_;
        
        std::vector<uint8_t> hashItem(const T& item);
    };
    
    template<typename K, typename V>
    class HashMap {
    public:
        explicit HashMap(HashAlgorithm algorithm = HashAlgorithm::SHA256);
        
        void insert(const K& key, const V& value);
        bool contains(const K& key) const;
        V get(const K& key) const;
        V getOrDefault(const K& key, const V& defaultValue) const;
        bool remove(const K& key);
        void clear();
        
        size_t size() const;
        bool empty() const;
        
        std::vector<K> getKeys() const;
        std::vector<V> getValues() const;
        std::vector<std::pair<K, V>> getItems() const;
        
    private:
        std::unique_ptr<HashFunction> hashFunction_;
        std::unordered_map<std::vector<uint8_t>, V> hashMap_;
        
        std::vector<uint8_t> hashKey(const K& key);
    };
    
    // Bloom filter
    class BloomFilter {
    public:
        BloomFilter(size_t capacity, double falsePositiveRate = 0.01, HashAlgorithm algorithm = HashAlgorithm::SHA256);
        
        void add(const std::string& item);
        bool mightContain(const std::string& item) const;
        void clear();
        
        size_t size() const;
        size_t capacity() const;
        double falsePositiveRate() const;
        
    private:
        std::vector<std::unique_ptr<HashFunction>> hashFunctions_;
        std::vector<bool> bitArray_;
        size_t capacity_;
        double falsePositiveRate_;
        size_t hashCount_;
        
        void setBit(size_t index);
        bool getBit(size_t index) const;
        std::vector<size_t> getHashIndices(const std::string& item) const;
    };
    
    // Count-Min Sketch
    class CountMinSketch {
    public:
        CountMinSketch(size_t width, size_t depth, HashAlgorithm algorithm = HashAlgorithm::SHA256);
        
        void add(const std::string& item, uint64_t count = 1);
        uint64_t estimate(const std::string& item) const;
        void clear();
        
        size_t width() const;
        size_t depth() const;
        
    private:
        std::vector<std::unique_ptr<HashFunction>> hashFunctions_;
        std::vector<std::vector<uint64_t>> table_;
        size_t width_;
        size_t depth_;
        
        std::vector<size_t> getHashIndices(const std::string& item) const;
    };
    
    // HyperLogLog
    class HyperLogLog {
    public:
        explicit HyperLogLog(uint8_t precision = 12, HashAlgorithm algorithm = HashAlgorithm::SHA256);
        
        void add(const std::string& item);
        uint64_t estimate() const;
        void clear();
        
        uint8_t precision() const;
        size_t registerCount() const;
        
        HyperLogLog merge(const HyperLogLog& other) const;
        
    private:
        std::unique_ptr<HashFunction> hashFunction_;
        std::vector<uint8_t> registers_;
        uint8_t precision_;
        size_t registerCount_;
        uint32_t mask_;
        
        uint8_t countLeadingZeros(uint64_t value) const;
        size_t getRegisterIndex(uint64_t hash) const;
        uint8_t getRegisterValue(uint64_t hash) const;
    };
}

// Template implementations
template<typename T>
void HashUtils::HashSet<T>::insert(const T& item) {
    auto hash = hashItem(item);
    hashSet_.insert(hash);
}

template<typename T>
bool HashUtils::HashSet<T>::contains(const T& item) const {
    auto hash = hashItem(item);
    return hashSet_.find(hash) != hashSet_.end();
}

template<typename T>
bool HashUtils::HashSet<T>::remove(const T& item) {
    auto hash = hashItem(item);
    return hashSet_.erase(hash) > 0;
}

template<typename T>
void HashUtils::HashSet<T>::clear() {
    hashSet_.clear();
}

template<typename T>
size_t HashUtils::HashSet<T>::size() const {
    return hashSet_.size();
}

template<typename T>
bool HashUtils::HashSet<T>::empty() const {
    return hashSet_.empty();
}

template<typename T>
std::vector<T> HashUtils::HashSet<T>::getItems() const {
    // This would require storing the original items or having a way to reconstruct them
    // For now, return empty vector as this is just a hash-based set
    return std::vector<T>();
}

template<typename T>
std::vector<uint8_t> HashUtils::HashSet<T>::hashItem(const T& item) {
    // This would need to be specialized for different types
    // For now, we'll use a simple approach
    if constexpr (std::is_same_v<T, std::string>) {
        return hashFunction_->hash(item);
    } else if constexpr (std::is_arithmetic_v<T>) {
        auto bytes = reinterpret_cast<const uint8_t*>(&item);
        return hashFunction_->hash(bytes, sizeof(T));
    } else {
        // For complex types, we'd need serialization
        static_assert(false, "Unsupported type for HashSet");
    }
}

template<typename K, typename V>
void HashUtils::HashMap<K, V>::insert(const K& key, const V& value) {
    auto hash = hashKey(key);
    hashMap_[hash] = value;
}

template<typename K, typename V>
bool HashUtils::HashMap<K, V>::contains(const K& key) const {
    auto hash = hashKey(key);
    return hashMap_.find(hash) != hashMap_.end();
}

template<typename K, typename V>
V HashUtils::HashMap<K, V>::get(const K& key) const {
    auto hash = hashKey(key);
    auto it = hashMap_.find(hash);
    if (it != hashMap_.end()) {
        return it->second;
    }
    throw std::out_of_range("Key not found");
}

template<typename K, typename V>
V HashUtils::HashMap<K, V>::getOrDefault(const K& key, const V& defaultValue) const {
    auto hash = hashKey(key);
    auto it = hashMap_.find(hash);
    if (it != hashMap_.end()) {
        return it->second;
    }
    return defaultValue;
}

template<typename K, typename V>
bool HashUtils::HashMap<K, V>::remove(const K& key) {
    auto hash = hashKey(key);
    return hashMap_.erase(hash) > 0;
}

template<typename K, typename V>
void HashUtils::HashMap<K, V>::clear() {
    hashMap_.clear();
}

template<typename K, typename V>
size_t HashUtils::HashMap<K, V>::size() const {
    return hashMap_.size();
}

template<typename K, typename V>
bool HashUtils::HashMap<K, V>::empty() const {
    return hashMap_.empty();
}

template<typename K, typename V>
std::vector<K> HashUtils::HashMap<K, V>::getKeys() const {
    // This would require storing the original keys or having a way to reconstruct them
    return std::vector<K>();
}

template<typename K, typename V>
std::vector<V> HashUtils::HashMap<K, V>::getValues() const {
    std::vector<V> values;
    values.reserve(hashMap_.size());
    for (const auto& pair : hashMap_) {
        values.push_back(pair.second);
    }
    return values;
}

template<typename K, typename V>
std::vector<std::pair<K, V>> HashUtils::HashMap<K, V>::getItems() const {
    // This would require storing the original keys or having a way to reconstruct them
    return std::vector<std::pair<K, V>>();
}

template<typename K, typename V>
std::vector<uint8_t> HashUtils::HashMap<K, V>::hashKey(const K& key) {
    // This would need to be specialized for different types
    if constexpr (std::is_same_v<K, std::string>) {
        return hashFunction_->hash(key);
    } else if constexpr (std::is_arithmetic_v<K>) {
        auto bytes = reinterpret_cast<const uint8_t*>(&key);
        return hashFunction_->hash(bytes, sizeof(K));
    } else {
        // For complex types, we'd need serialization
        static_assert(false, "Unsupported type for HashMap key");
    }
}

} // namespace RFUtils