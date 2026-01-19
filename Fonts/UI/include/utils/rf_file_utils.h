#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <functional>

namespace RFUtils {

// Forward declarations
class Vector2;
class Vector3;
class Vector4;

// File types
enum class FileType {
    Unknown,
    Regular,
    Directory,
    Symlink,
    Character,
    Block,
    FIFO,
    Socket
};

// File permissions
enum class FilePermission {
    None = 0,
    OwnerRead = 1 << 0,
    OwnerWrite = 1 << 1,
    OwnerExecute = 1 << 2,
    GroupRead = 1 << 3,
    GroupWrite = 1 << 4,
    GroupExecute = 1 << 5,
    OtherRead = 1 << 6,
    OtherWrite = 1 << 7,
    OtherExecute = 1 << 8,
    All = OwnerRead | OwnerWrite | OwnerExecute | GroupRead | GroupWrite | GroupExecute | OtherRead | OtherWrite | OtherExecute
};

// File attributes
enum class FileAttribute {
    None = 0,
    ReadOnly = 1 << 0,
    Hidden = 1 << 1,
    System = 1 << 2,
    Archive = 1 << 3,
    Temporary = 1 << 4,
    Compressed = 1 << 5,
    Encrypted = 1 << 6,
    Sparse = 1 << 7,
    ReparsePoint = 1 << 8,
    Offline = 1 << 9,
    NotContentIndexed = 1 << 10,
    IntegrityStream = 1 << 11,
    Virtual = 1 << 12,
    NoScrubData = 1 << 13,
    NoScrubDirectory = 1 << 14
};

// File comparison result
enum class FileComparison {
    Identical,
    Different,
    Newer,
    Older,
    SameSize,
    SameContent,
    Error
};

// File operation result
enum class FileOperationResult {
    Success,
    Failure,
    NotFound,
    AccessDenied,
    AlreadyExists,
    InsufficientSpace,
    InvalidPath,
    NetworkError,
    Timeout,
    Cancelled
};

// File class
class File {
public:
    // Constructors
    File();
    File(const std::string& path);
    File(const std::filesystem::path& path);
    
    // Copy constructor
    File(const File& other);
    
    // Assignment operator
    File& operator=(const File& other);
    
    // Path operations
    std::string getPath() const;
    std::string getAbsolutePath() const;
    std::string getParentPath() const;
    std::string getFileName() const;
    std::string getFileNameWithoutExtension() const;
    std::string getFileExtension() const;
    std::string getStem() const;
    
    void setPath(const std::string& path);
    void setPath(const std::filesystem::path& path);
    
    // File information
    bool exists() const;
    FileType getType() const;
    uint64_t getSize() const;
    std::chrono::system_clock::time_point getCreationTime() const;
    std::chrono::system_clock::time_point getModificationTime() const;
    std::chrono::system_clock::time_point getAccessTime() const;
    
    FilePermission getPermissions() const;
    FileAttribute getAttributes() const;
    std::string getOwner() const;
    std::string getGroup() const;
    
    // File operations
    FileOperationResult create(bool overwrite = false);
    FileOperationResult remove();
    FileOperationResult copy(const std::string& destination, bool overwrite = false);
    FileOperationResult move(const std::string& destination, bool overwrite = false);
    FileOperationResult rename(const std::string& newName);
    
    // Directory operations
    FileOperationResult createDirectory();
    FileOperationResult removeDirectory();
    FileOperationResult createDirectories();
    
    // File content operations
    std::vector<uint8_t> readBytes() const;
    std::string readText() const;
    std::vector<std::string> readLines() const;
    
    FileOperationResult writeBytes(const std::vector<uint8_t>& data);
    FileOperationResult writeText(const std::string& text);
    FileOperationResult writeLines(const std::vector<std::string>& lines);
    
    FileOperationResult appendBytes(const std::vector<uint8_t>& data);
    FileOperationResult appendText(const std::string& text);
    FileOperationResult appendLine(const std::string& line);
    
    // File stream operations
    std::ifstream openInputStream(std::ios_base::openmode mode = std::ios::in) const;
    std::ofstream openOutputStream(std::ios_base::openmode mode = std::ios::out) const;
    std::fstream openStream(std::ios_base::openmode mode = std::ios::in | std::ios::out) const;
    
    // File comparison
    FileComparison compare(const File& other) const;
    bool isIdentical(const File& other) const;
    bool isNewer(const File& other) const;
    bool isOlder(const File& other) const;
    
    // Utility methods
    bool isReadable() const;
    bool isWritable() const;
    bool isExecutable() const;
    bool isHidden() const;
    bool isSystem() const;
    bool isArchive() const;
    bool isTemporary() const;
    bool isCompressed() const;
    bool isEncrypted() const;
    
    bool isEmpty() const;
    bool isDirectory() const;
    bool isRegular() const;
    bool isSymlink() const;
    
    // Validation
    bool isValid() const;
    bool isValidPath() const;
    
    // String conversion
    std::string toString() const;
    
    // Static methods
    static File getCurrentDirectory();
    static File getHomeDirectory();
    static File getTempDirectory();
    static File getApplicationDirectory();
    static File getDocumentsDirectory();
    static File getDesktopDirectory();
    static File getDownloadsDirectory();
    static File getMusicDirectory();
    static File getPicturesDirectory();
    static File getVideosDirectory();
    
    static File combine(const std::string& path1, const std::string& path2);
    static File combine(const std::vector<std::string>& paths);
    
    static File getRelativePath(const File& from, const File& to);
    static File getAbsolutePath(const File& file);
    
    // Constants
    static constexpr uint64_t INVALID_SIZE = static_cast<uint64_t>(-1);
    static constexpr FilePermission DEFAULT_PERMISSIONS = FilePermission::OwnerRead | FilePermission::OwnerWrite | FilePermission::GroupRead | FilePermission::OtherRead;
    
private:
    std::filesystem::path path_;
    
    void validatePath() const;
    std::filesystem::file_status getStatus() const;
};

// File utilities namespace
namespace FileUtils {
    // File operations
    FileOperationResult createFile(const std::string& path, bool overwrite = false);
    FileOperationResult createDirectory(const std::string& path);
    FileOperationResult createDirectories(const std::string& path);
    
    FileOperationResult removeFile(const std::string& path);
    FileOperationResult removeDirectory(const std::string& path, bool recursive = false);
    FileOperationResult removeTree(const std::string& path);
    
    FileOperationResult copyFile(const std::string& source, const std::string& destination, bool overwrite = false);
    FileOperationResult copyDirectory(const std::string& source, const std::string& destination, bool overwrite = false);
    FileOperationResult copyTree(const std::string& source, const std::string& destination, bool overwrite = false);
    
    FileOperationResult moveFile(const std::string& source, const std::string& destination, bool overwrite = false);
    FileOperationResult moveDirectory(const std::string& source, const std::string& destination, bool overwrite = false);
    FileOperationResult moveTree(const std::string& source, const std::string& destination, bool overwrite = false);
    
    FileOperationResult renameFile(const std::string& oldPath, const std::string& newPath);
    FileOperationResult renameDirectory(const std::string& oldPath, const std::string& newPath);
    
    // File information
    bool fileExists(const std::string& path);
    bool directoryExists(const std::string& path);
    bool isFile(const std::string& path);
    bool isDirectory(const std::string& path);
    bool isSymlink(const std::string& path);
    bool isReadable(const std::string& path);
    bool isWritable(const std::string& path);
    bool isExecutable(const std::string& path);
    bool isHidden(const std::string& path);
    
    FileType getFileType(const std::string& path);
    uint64_t getFileSize(const std::string& path);
    std::chrono::system_clock::time_point getCreationTime(const std::string& path);
    std::chrono::system_clock::time_point getModificationTime(const std::string& path);
    std::chrono::system_clock::time_point getAccessTime(const std::string& path);
    
    FilePermission getPermissions(const std::string& path);
    FileAttribute getAttributes(const std::string& path);
    std::string getOwner(const std::string& path);
    std::string getGroup(const std::string& path);
    
    // Path operations
    std::string getAbsolutePath(const std::string& path);
    std::string getRelativePath(const std::string& from, const std::string& to);
    std::string getParentPath(const std::string& path);
    std::string getFileName(const std::string& path);
    std::string getFileNameWithoutExtension(const std::string& path);
    std::string getFileExtension(const std::string& path);
    std::string getStem(const std::string& path);
    
    std::string combinePaths(const std::string& path1, const std::string& path2);
    std::string combinePaths(const std::vector<std::string>& paths);
    
    std::string normalizePath(const std::string& path);
    std::string canonicalPath(const std::string& path);
    
    // Directory operations
    std::vector<std::string> listFiles(const std::string& directory, const std::string& pattern = "*");
    std::vector<std::string> listDirectories(const std::string& directory, const std::string& pattern = "*");
    std::vector<std::string> listAll(const std::string& directory, const std::string& pattern = "*");
    
    std::vector<std::string> listFilesRecursive(const std::string& directory, const std::string& pattern = "*");
    std::vector<std::string> listDirectoriesRecursive(const std::string& directory, const std::string& pattern = "*");
    std::vector<std::string> listAllRecursive(const std::string& directory, const std::string& pattern = "*");
    
    // File search
    std::vector<std::string> findFiles(const std::string& directory, const std::string& pattern, bool recursive = false);
    std::vector<std::string> findDirectories(const std::string& directory, const std::string& pattern, bool recursive = false);
    std::vector<std::string> findAll(const std::string& directory, const std::string& pattern, bool recursive = false);
    
    std::string findFile(const std::string& directory, const std::string& fileName, bool recursive = false);
    std::string findDirectory(const std::string& directory, const std::string& dirName, bool recursive = false);
    
    // File content operations
    std::vector<uint8_t> readBytes(const std::string& path);
    std::string readText(const std::string& path);
    std::vector<std::string> readLines(const std::string& path);
    
    FileOperationResult writeBytes(const std::string& path, const std::vector<uint8_t>& data);
    FileOperationResult writeText(const std::string& path, const std::string& text);
    FileOperationResult writeLines(const std::string& path, const std::vector<std::string>& lines);
    
    FileOperationResult appendBytes(const std::string& path, const std::vector<uint8_t>& data);
    FileOperationResult appendText(const std::string& path, const std::string& text);
    FileOperationResult appendLine(const std::string& path, const std::string& line);
    
    // File comparison
    FileComparison compareFiles(const std::string& path1, const std::string& path2);
    bool filesAreIdentical(const std::string& path1, const std::string& path2);
    bool filesHaveSameSize(const std::string& path1, const std::string& path2);
    bool filesHaveSameContent(const std::string& path1, const std::string& path2);
    
    // File validation
    bool isValidPath(const std::string& path);
    bool isValidFileName(const std::string& fileName);
    bool isValidDirectoryName(const std::string& dirName);
    
    // File permissions
    FileOperationResult setPermissions(const std::string& path, FilePermission permissions);
    FileOperationResult addPermission(const std::string& path, FilePermission permission);
    FileOperationResult removePermission(const std::string& path, FilePermission permission);
    
    // File attributes
    FileOperationResult setAttributes(const std::string& path, FileAttribute attributes);
    FileOperationResult addAttribute(const std::string& path, FileAttribute attribute);
    FileOperationResult removeAttribute(const std::string& path, FileAttribute attribute);
    
    FileOperationResult setHidden(const std::string& path, bool hidden);
    FileOperationResult setReadOnly(const std::string& path, bool readOnly);
    FileOperationResult setSystem(const std::string& path, bool system);
    FileOperationResult setArchive(const std::string& path, bool archive);
    
    // File monitoring
    class FileWatcher {
    public:
        enum class EventType {
            Created,
            Modified,
            Deleted,
            Renamed,
            AttributeChanged
        };
        
        struct Event {
            EventType type;
            std::string path;
            std::string oldPath;
            std::chrono::system_clock::time_point timestamp;
        };
        
        FileWatcher();
        FileWatcher(const std::string& path, bool recursive = false);
        ~FileWatcher();
        
        bool start();
        bool stop();
        bool isRunning() const;
        
        void setPath(const std::string& path);
        void setRecursive(bool recursive);
        
        std::vector<Event> getEvents();
        bool hasEvents() const;
        void clearEvents();
        
        void setEventCallback(std::function<void(const Event&)> callback);
        
    private:
        std::string path_;
        bool recursive_;
        bool running_;
        std::vector<Event> events_;
        std::function<void(const Event&)> callback_;
        
        void monitor();
        void processChanges();
    };
    
    // File cache
    class FileCache {
    public:
        FileCache(size_t maxSize = 1000);
        
        void put(const std::string& key, const std::string& data);
        std::string get(const std::string& key);
        bool contains(const std::string& key) const;
        void remove(const std::string& key);
        void clear();
        
        size_t size() const;
        size_t maxSize() const;
        void setMaxSize(size_t maxSize);
        
        double getHitRate() const;
        size_t getHitCount() const;
        size_t getMissCount() const;
        
    private:
        struct CacheEntry {
            std::string data;
            size_t accessCount;
            uint64_t lastAccessTime;
        };
        
        std::map<std::string, CacheEntry> cache_;
        size_t maxSize_;
        size_t hitCount_;
        size_t missCount_;
        
        void evictLeastRecentlyUsed();
    };
    
    // File pool
    class FilePool {
    public:
        FilePool(size_t initialSize = 100);
        
        std::fstream* acquire();
        void release(std::fstream* file);
        void clear();
        
        size_t getPoolSize() const;
        size_t getUsedCount() const;
        size_t getAvailableCount() const;
        
    private:
        std::vector<std::unique_ptr<std::fstream>> pool_;
        std::vector<std::fstream*> available_;
        std::vector<std::fstream*> used_;
        
        void expandPool(size_t newSize);
    };
    
    // File utilities
    class FileUtilities {
    public:
        // File operations
        static FileOperationResult createBackup(const std::string& path, const std::string& backupPath = "");
        static FileOperationResult restoreBackup(const std::string& backupPath, const std::string& targetPath = "");
        
        static FileOperationResult compressFile(const std::string& path, const std::string& compressedPath = "");
        static FileOperationResult decompressFile(const std::string& compressedPath, const std::string& targetPath = "");
        
        static FileOperationResult encryptFile(const std::string& path, const std::string& key, const std::string& encryptedPath = "");
        static FileOperationResult decryptFile(const std::string& encryptedPath, const std::string& key, const std::string& targetPath = "");
        
        // File analysis
        static std::string calculateHash(const std::string& path, const std::string& algorithm = "MD5");
        static std::string calculateMD5(const std::string& path);
        static std::string calculateSHA1(const std::string& path);
        static std::string calculateSHA256(const std::string& path);
        
        static std::vector<std::string> calculateHashes(const std::vector<std::string>& paths, const std::string& algorithm = "MD5");
        
        // File synchronization
        static FileOperationResult synchronizeDirectories(const std::string& source, const std::string& destination, bool recursive = false);
        static FileOperationResult synchronizeFiles(const std::string& source, const std::string& destination);
        
        // File archiving
        static FileOperationResult createArchive(const std::vector<std::string>& files, const std::string& archivePath);
        static FileOperationResult extractArchive(const std::string& archivePath, const std::string& destinationPath);
        
        static std::vector<std::string> listArchiveContents(const std::string& archivePath);
        
        // File validation
        static bool validateFile(const std::string& path);
        static bool validateFileIntegrity(const std::string& path, const std::string& expectedHash);
        static bool validateFilePermissions(const std::string& path, FilePermission requiredPermissions);
        
        // File cleanup
        static FileOperationResult cleanupTemporaryFiles(const std::string& directory);
        static FileOperationResult cleanupOldFiles(const std::string& directory, std::chrono::system_clock::duration maxAge);
        static FileOperationResult cleanupEmptyDirectories(const std::string& directory);
        
        // File statistics
        struct DirectoryStatistics {
            size_t fileCount;
            size_t directoryCount;
            size_t totalSize;
            size_t averageFileSize;
            size_t largestFileSize;
            size_t smallestFileSize;
            std::string largestFile;
            std::string smallestFile;
            std::chrono::system_clock::time_point oldestFile;
            std::chrono::system_clock::time_point newestFile;
        };
        
        static DirectoryStatistics getDirectoryStatistics(const std::string& directory, bool recursive = false);
        
        // File search utilities
        class FileSearcher {
        public:
            enum class SearchType {
                FileName,
                FileContent,
                FileSize,
                FileDate,
                FileExtension,
                FileAttribute
            };
            
            FileSearcher();
            
            void addSearchCriteria(SearchType type, const std::string& criteria);
            void addSearchCriteria(SearchType type, int64_t value);
            void addSearchCriteria(SearchType type, std::chrono::system_clock::time_point value);
            
            std::vector<std::string> search(const std::string& directory, bool recursive = false);
            
            void clearCriteria();
            
        private:
            struct SearchCriterion {
                SearchType type;
                std::string stringValue;
                int64_t intValue;
                std::chrono::system_clock::time_point dateValue;
            };
            
            std::vector<SearchCriterion> criteria_;
            
            bool matchesCriteria(const std::string& path) const;
        };
        
        // File batch operations
        class FileBatchProcessor {
        public:
            enum class Operation {
                Copy,
                Move,
                Delete,
                Compress,
                Encrypt,
                CalculateHash
            };
            
            FileBatchProcessor();
            
            void addFile(const std::string& path);
            void addFiles(const std::vector<std::string>& paths);
            void addDirectory(const std::string& directory, bool recursive = false);
            
            void setOperation(Operation operation);
            void setDestination(const std::string& destination);
            void setOverwrite(bool overwrite);
            void setKey(const std::string& key);
            void setAlgorithm(const std::string& algorithm);
            
            std::vector<FileOperationResult> process();
            
            void clearFiles();
            
        private:
            std::vector<std::string> files_;
            Operation operation_;
            std::string destination_;
            bool overwrite_;
            std::string key_;
            std::string algorithm_;
            
            FileOperationResult processFile(const std::string& path);
        };
        
        // File backup utilities
        class BackupManager {
        public:
            BackupManager(const std::string& backupDirectory);
            
            FileOperationResult createBackup(const std::string& source, const std::string& backupName = "");
            FileOperationResult restoreBackup(const std::string& backupName, const std::string& target);
            
            std::vector<std::string> listBackups();
            FileOperationResult deleteBackup(const std::string& backupName);
            
            FileOperationResult cleanupOldBackups(std::chrono::system_clock::duration maxAge);
            FileOperationResult cleanupBackups(size_t maxCount);
            
        private:
            std::string backupDirectory_;
            
            std::string getBackupPath(const std::string& backupName);
            std::string generateBackupName();
        };
        
        // File logging utilities
        class FileLogger {
        public:
            enum class LogLevel {
                Debug,
                Info,
                Warning,
                Error,
                Fatal
            };
            
            FileLogger(const std::string& logPath);
            ~FileLogger();
            
            void log(LogLevel level, const std::string& message);
            void debug(const std::string& message);
            void info(const std::string& message);
            void warning(const std::string& message);
            void error(const std::string& message);
            void fatal(const std::string& message);
            
            void setLogLevel(LogLevel level);
            void setMaxFileSize(size_t maxSize);
            void setMaxFiles(size_t maxFiles);
            
        private:
            std::string logPath_;
            LogLevel logLevel_;
            size_t maxFileSize_;
            size_t maxFiles_;
            
            void rotateLog();
            std::string getCurrentLogFileName();
            std::string formatMessage(LogLevel level, const std::string& message);
        };
    };
    
    // File system utilities
    namespace FileSystemUtils {
        // Drive operations
        std::vector<std::string> getAvailableDrives();
        std::string getDriveType(const std::string& drive);
        uint64_t getDriveFreeSpace(const std::string& drive);
        uint64_t getDriveTotalSpace(const std::string& drive);
        uint64_t getDriveUsedSpace(const std::string& drive);
        
        // Volume operations
        std::string getVolumeLabel(const std::string& drive);
        FileOperationResult setVolumeLabel(const std::string& drive, const std::string& label);
        
        std::string getFileSystemType(const std::string& drive);
        
        // Mount operations
        std::vector<std::string> getMountedVolumes();
        std::string getMountPoint(const std::string& volume);
        FileOperationResult mountVolume(const std::string& volume, const std::string& mountPoint);
        FileOperationResult unmountVolume(const std::string& mountPoint);
        
        // Network operations
        std::vector<std::string> getNetworkShares();
        std::vector<std::string> getNetworkDrives();
        
        FileOperationResult mapNetworkDrive(const std::string& share, const std::string& drive);
        FileOperationResult unmapNetworkDrive(const std::string& drive);
        
        // System operations
        std::string getSystemDirectory();
        std::string getProgramFilesDirectory();
        std::string getProgramFilesX86Directory();
        std::string getCommonFilesDirectory();
        std::string getCommonAppDataDirectory();
        std::string getUserAppDataDirectory();
        std::string getUserLocalAppDataDirectory();
        std::string getUserDesktopDirectory();
        std::string getUserDocumentsDirectory();
        std::string getUserDownloadsDirectory();
        std::string getUserMusicDirectory();
        std::string getUserPicturesDirectory();
        std::string getUserVideosDirectory();
        
        // Path utilities
        std::string getTempPath();
        std::string getCurrentPath();
        std::string getExecutablePath();
        std::string getModulePath();
        
        // Environment utilities
        std::string getEnvironmentVariable(const std::string& name);
        std::vector<std::string> getEnvironmentVariables();
        
        FileOperationResult setEnvironmentVariable(const std::string& name, const std::string& value);
        FileOperationResult deleteEnvironmentVariable(const std::string& name);
        
        // Special folders
        std::string getSpecialFolder(int32_t folderId);
        std::vector<std::string> getSpecialFolders();
    }
    
    // File format utilities
    namespace FileFormatUtils {
        // Image formats
        bool isImageFile(const std::string& path);
        std::string getImageFormat(const std::string& path);
        std::vector<std::string> getSupportedImageFormats();
        
        // Audio formats
        bool isAudioFile(const std::string& path);
        std::string getAudioFormat(const std::string& path);
        std::vector<std::string> getSupportedAudioFormats();
        
        // Video formats
        bool isVideoFile(const std::string& path);
        std::string getVideoFormat(const std::string& path);
        std::vector<std::string> getSupportedVideoFormats();
        
        // Document formats
        bool isDocumentFile(const std::string& path);
        std::string getDocumentFormat(const std::string& path);
        std::vector<std::string> getSupportedDocumentFormats();
        
        // Archive formats
        bool isArchiveFile(const std::string& path);
        std::string getArchiveFormat(const std::string& path);
        std::vector<std::string> getSupportedArchiveFormats();
        
        // Text formats
        bool isTextFile(const std::string& path);
        std::string getTextFormat(const std::string& path);
        std::vector<std::string> getSupportedTextFormats();
        
        // Binary formats
        bool isBinaryFile(const std::string& path);
        std::string getBinaryFormat(const std::string& path);
        std::vector<std::string> getSupportedBinaryFormats();
        
        // Format detection
        std::string detectFileFormat(const std::string& path);
        std::string detectFileFormatFromContent(const std::string& path);
        std::string detectFileFormatFromExtension(const std::string& path);
        
        // Format conversion
        FileOperationResult convertFile(const std::string& sourcePath, const std::string& targetPath, const std::string& targetFormat);
        std::vector<std::string> getSupportedConversions(const std::string& sourceFormat);
    }
    
    // File validation utilities
    namespace FileValidationUtils {
        // Path validation
        bool isValidPath(const std::string& path);
        bool isValidFileName(const std::string& fileName);
        bool isValidDirectoryName(const std::string& dirName);
        bool isValidExtension(const std::string& extension);
        
        // Path sanitization
        std::string sanitizePath(const std::string& path);
        std::string sanitizeFileName(const std::string& fileName);
        std::string sanitizeDirectoryName(const std::string& dirName);
        
        // Path security
        bool isPathSafe(const std::string& path);
        bool isPathTraversing(const std::string& path);
        bool isPathAbsolute(const std::string& path);
        bool isPathRelative(const std::string& path);
        
        // File size validation
        bool isValidFileSize(uint64_t size);
        bool isValidFileSize(const std::string& path);
        
        // File content validation
        bool isValidTextFile(const std::string& path);
        bool isValidBinaryFile(const std::string& path);
        bool isValidImageFile(const std::string& path);
        bool isValidAudioFile(const std::string& path);
        bool isValidVideoFile(const std::string& path);
        
        // File integrity validation
        bool validateFileIntegrity(const std::string& path, const std::string& expectedHash);
        bool validateFileIntegrity(const std::string& path, const std::vector<std::string>& expectedHashes);
        
        // File permission validation
        bool hasReadPermission(const std::string& path);
        bool hasWritePermission(const std::string& path);
        bool hasExecutePermission(const std::string& path);
        bool hasPermission(const std::string& path, FilePermission permission);
        
        // File attribute validation
        bool hasAttribute(const std::string& path, FileAttribute attribute);
        bool isHiddenFile(const std::string& path);
        bool isSystemFile(const std::string& path);
        bool isArchiveFile(const std::string& path);
        bool isTemporaryFile(const std::string& path);
        
        // File name validation
        bool containsInvalidCharacters(const std::string& fileName);
        bool containsReservedNames(const std::string& fileName);
        bool hasValidLength(const std::string& fileName);
        
        // File system validation
        bool isValidFileSystem(const std::string& path);
        bool isSupportedFileSystem(const std::string& path);
        bool hasEnoughSpace(const std::string& path, uint64_t requiredSpace);
        
        // File validation rules
        class ValidationRule {
        public:
            virtual ~ValidationRule() = default;
            virtual bool validate(const std::string& path) const = 0;
            virtual std::string getErrorMessage() const = 0;
        };
        
        class PathValidationRule : public ValidationRule {
        public:
            PathValidationRule();
            bool validate(const std::string& path) const override;
            std::string getErrorMessage() const override;
        };
        
        class FileNameValidationRule : public ValidationRule {
        public:
            FileNameValidationRule();
            bool validate(const std::string& path) const override;
            std::string getErrorMessage() const override;
        };
        
        class FileSizeValidationRule : public ValidationRule {
        public:
            FileSizeValidationRule(uint64_t maxSize);
            bool validate(const std::string& path) const override;
            std::string getErrorMessage() const override;
            
        private:
            uint64_t maxSize_;
        };
        
        class FileExtensionValidationRule : public ValidationRule {
        public:
            FileExtensionValidationRule(const std::vector<std::string>& allowedExtensions);
            bool validate(const std::string& path) const override;
            std::string getErrorMessage() const override;
            
        private:
            std::vector<std::string> allowedExtensions_;
        };
        
        class FileValidator {
        public:
            FileValidator();
            
            void addRule(std::unique_ptr<ValidationRule> rule);
            void removeRule(size_t index);
            void clearRules();
            
            bool validate(const std::string& path) const;
            std::vector<std::string> getErrorMessages(const std::string& path) const;
            
        private:
            std::vector<std::unique_ptr<ValidationRule>> rules_;
        };
    }
}

// Bitwise operators for FilePermission
inline FilePermission operator|(FilePermission a, FilePermission b) {
    return static_cast<FilePermission>(static_cast<int32_t>(a) | static_cast<int32_t>(b));
}

inline FilePermission operator&(FilePermission a, FilePermission b) {
    return static_cast<FilePermission>(static_cast<int32_t>(a) & static_cast<int32_t>(b));
}

inline FilePermission operator^(FilePermission a, FilePermission b) {
    return static_cast<FilePermission>(static_cast<int32_t>(a) ^ static_cast<int32_t>(b));
}

inline FilePermission operator~(FilePermission a) {
    return static_cast<FilePermission>(~static_cast<int32_t>(a));
}

inline FilePermission& operator|=(FilePermission& a, FilePermission b) {
    a = a | b;
    return a;
}

inline FilePermission& operator&=(FilePermission& a, FilePermission b) {
    a = a & b;
    return a;
}

inline FilePermission& operator^=(FilePermission& a, FilePermission b) {
    a = a ^ b;
    return a;
}

// Bitwise operators for FileAttribute
inline FileAttribute operator|(FileAttribute a, FileAttribute b) {
    return static_cast<FileAttribute>(static_cast<int32_t>(a) | static_cast<int32_t>(b));
}

inline FileAttribute operator&(FileAttribute a, FileAttribute b) {
    return static_cast<FileAttribute>(static_cast<int32_t>(a) & static_cast<int32_t>(b));
}

inline FileAttribute operator^(FileAttribute a, FileAttribute b) {
    return static_cast<FileAttribute>(static_cast<int32_t>(a) ^ static_cast<int32_t>(b));
}

inline FileAttribute operator~(FileAttribute a) {
    return static_cast<FileAttribute>(~static_cast<int32_t>(a));
}

inline FileAttribute& operator|=(FileAttribute& a, FileAttribute b) {
    a = a | b;
    return a;
}

inline FileAttribute& operator&=(FileAttribute& a, FileAttribute b) {
    a = a & b;
    return a;
}

inline FileAttribute& operator^=(FileAttribute& a, FileAttribute b) {
    a = a ^ b;
    return a;
}

} // namespace RFUtils