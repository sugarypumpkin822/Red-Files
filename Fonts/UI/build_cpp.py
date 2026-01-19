#!/usr/bin/env python3
"""
Red Files C++ Font Compiler Builder
Detects and builds the C++ font compiler using multiple available compilers.
"""

import os
import sys
import subprocess
import shutil
import platform
from pathlib import Path
from typing import List, Dict, Optional, Tuple

class CompilerDetector:
    """Detects available C++ compilers on the system"""
    
    def __init__(self):
        self.compilers = []
        self.system = platform.system().lower()
        self.script_dir = Path(__file__).parent
        self.build_dir = self.script_dir / "build"
        self.bin_dir = self.build_dir / "bin"
        
    def detect_compilers(self) -> List[Dict]:
        """Detect all available C++ compilers"""
        compilers = []
        
        if self.system == "windows":
            compilers.extend(self._detect_windows_compilers())
        else:
            compilers.extend(self._detect_unix_compilers())
            
        self.compilers = compilers
        return compilers
    
    def _detect_windows_compilers(self) -> List[Dict]:
        """Detect Windows compilers"""
        compilers = []
        
        # Visual Studio (MSVC)
        vs_compilers = self._detect_visual_studio()
        compilers.extend(vs_compilers)
        
        # MinGW/GCC
        mingw = self._detect_mingw()
        if mingw:
            compilers.append(mingw)
            
        # Clang on Windows
        clang = self._detect_clang_windows()
        if clang:
            compilers.append(clang)
            
        return compilers
    
    def _detect_unix_compilers(self) -> List[Dict]:
        """Detect Unix-like system compilers (Linux/macOS)"""
        compilers = []
        
        # GCC
        gcc = self._detect_gcc()
        if gcc:
            compilers.append(gcc)
            
        # Clang
        clang = self._detect_clang_unix()
        if clang:
            compilers.append(clang)
            
        # Intel C++ Compiler
        intel = self._detect_intel_compiler()
        if intel:
            compilers.append(intel)
            
        return compilers
    
    def _detect_visual_studio(self) -> List[Dict]:
        """Detect Visual Studio installations"""
        compilers = []
        
        # Check for cl.exe in PATH
        cl_path = shutil.which("cl")
        if cl_path:
            # Try to get version
            try:
                result = subprocess.run(["cl"], capture_output=True, text=True, timeout=5)
                if result.returncode == 0:
                    version = self._extract_vs_version(result.stderr)
                    compilers.append({
                        "name": "Visual Studio C++",
                        "command": "cl",
                        "version": version,
                        "type": "msvc",
                        "priority": 1,
                        "path": cl_path
                    })
            except:
                compilers.append({
                    "name": "Visual Studio C++",
                    "command": "cl",
                    "version": "Unknown",
                    "type": "msvc",
                    "priority": 1,
                    "path": cl_path
                })
        
        # Check common VS installation paths
        vs_paths = [
            r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat",
            r"C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat",
            r"C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat",
            r"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat",
        ]
        
        for vs_path in vs_paths:
            if os.path.exists(vs_path):
                compilers.append({
                    "name": "Visual Studio C++",
                    "command": "cl",
                    "version": "2022",
                    "type": "msvc",
                    "priority": 1,
                    "path": vs_path,
                    "setup": vs_path
                })
                break  # Use the first one found
                
        return compilers
    
    def _detect_mingw(self) -> Optional[Dict]:
        """Detect MinGW/GCC on Windows"""
        gcc_path = shutil.which("gcc")
        gxx_path = shutil.which("g++")
        
        if gcc_path and gxx_path:
            try:
                result = subprocess.run(["g++", "--version"], capture_output=True, text=True, timeout=5)
                version = result.stdout.split('\n')[0] if result.stdout else "Unknown"
                return {
                    "name": "MinGW/GCC",
                    "command": "g++",
                    "version": version,
                    "type": "gcc",
                    "priority": 2,
                    "path": gxx_path
                }
            except:
                return {
                    "name": "MinGW/GCC",
                    "command": "g++",
                    "version": "Unknown",
                    "type": "gcc",
                    "priority": 2,
                    "path": gxx_path
                }
        return None
    
    def _detect_clang_windows(self) -> Optional[Dict]:
        """Detect Clang on Windows"""
        clang_path = shutil.which("clang++")
        if clang_path:
            try:
                result = subprocess.run(["clang++", "--version"], capture_output=True, text=True, timeout=5)
                version = result.stdout.split('\n')[0] if result.stdout else "Unknown"
                return {
                    "name": "Clang",
                    "command": "clang++",
                    "version": version,
                    "type": "clang",
                    "priority": 2,
                    "path": clang_path
                }
            except:
                return {
                    "name": "Clang",
                    "command": "clang++",
                    "version": "Unknown",
                    "type": "clang",
                    "priority": 2,
                    "path": clang_path
                }
        return None
    
    def _detect_gcc(self) -> Optional[Dict]:
        """Detect GCC on Unix systems"""
        gcc_path = shutil.which("g++")
        if gcc_path:
            try:
                result = subprocess.run(["g++", "--version"], capture_output=True, text=True, timeout=5)
                version = result.stdout.split('\n')[0] if result.stdout else "Unknown"
                return {
                    "name": "GCC",
                    "command": "g++",
                    "version": version,
                    "type": "gcc",
                    "priority": 2,
                    "path": gcc_path
                }
            except:
                return {
                    "name": "GCC",
                    "command": "g++",
                    "version": "Unknown",
                    "type": "gcc",
                    "priority": 2,
                    "path": gcc_path
                }
        return None
    
    def _detect_clang_unix(self) -> Optional[Dict]:
        """Detect Clang on Unix systems"""
        clang_path = shutil.which("clang++")
        if clang_path:
            try:
                result = subprocess.run(["clang++", "--version"], capture_output=True, text=True, timeout=5)
                version = result.stdout.split('\n')[0] if result.stdout else "Unknown"
                return {
                    "name": "Clang",
                    "command": "clang++",
                    "version": version,
                    "type": "clang",
                    "priority": 1,  # Clang often preferred on macOS
                    "path": clang_path
                }
            except:
                return {
                    "name": "Clang",
                    "command": "clang++",
                    "version": "Unknown",
                    "type": "clang",
                    "priority": 1,
                    "path": clang_path
                }
        return None
    
    def _detect_intel_compiler(self) -> Optional[Dict]:
        """Detect Intel C++ Compiler"""
        icc_path = shutil.which("icpc")
        if icc_path:
            try:
                result = subprocess.run(["icpc", "--version"], capture_output=True, text=True, timeout=5)
                version = result.stdout.split('\n')[0] if result.stdout else "Unknown"
                return {
                    "name": "Intel C++ Compiler",
                    "command": "icpc",
                    "version": version,
                    "type": "intel",
                    "priority": 1,
                    "path": icc_path
                }
            except:
                return {
                    "name": "Intel C++ Compiler",
                    "command": "icpc",
                    "version": "Unknown",
                    "type": "intel",
                    "priority": 1,
                    "path": icc_path
                }
        return None
    
    def _extract_vs_version(self, vs_output: str) -> str:
        """Extract Visual Studio version from output"""
        import re
        match = re.search(r'Version (\d+\.\d+\.\d+)', vs_output)
        if match:
            return match.group(1)
        return "Unknown"


class CppBuilder:
    """Builds the C++ font compiler using detected compilers"""
    
    def __init__(self, detector: CompilerDetector):
        self.detector = detector
        self.source_files = [
            "tools/font_compiler_main.cpp",
            "integration/rf_font_api.cpp",
            "integration/rf_blood_api.cpp",
            "integration/rf_integration.cpp",
            "src/core/rf_font.c",
            "src/core/rf_font_cache.cpp",
            "src/core/rf_character_map.cpp"
        ]
        
    def build_with_best_compiler(self) -> bool:
        """Attempt to build with the best available compiler"""
        compilers = self.detector.detect_compilers()
        
        if not compilers:
            print("[ERROR] No C++ compilers found!")
            print("Please install one of the following:")
            if self.detector.system == "windows":
                print("  - Visual Studio (https://visualstudio.microsoft.com)")
                print("  - MinGW-w64 (https://www.mingw-w64.org)")
                print("  - Clang for Windows (https://releases.llvm.org)")
            else:
                print("  - GCC (sudo apt-get install build-essential)")
                print("  - Clang (sudo apt-get install clang)")
            return False
        
        # Sort by priority
        compilers.sort(key=lambda x: x["priority"])
        
        print(f"[INFO] Found {len(compilers)} C++ compiler(s):")
        for i, compiler in enumerate(compilers, 1):
            print(f"  {i}. {compiler['name']} v{compiler['version']}")
        
        # Try each compiler in order of preference
        for compiler in compilers:
            print(f"\n[INFO] Trying {compiler['name']}...")
            if self._build_with_compiler(compiler):
                print(f"[OK] Successfully built with {compiler['name']}!")
                return True
            else:
                print(f"[WARNING] Failed to build with {compiler['name']}")
        
        print("[ERROR] All compilers failed!")
        return False
    
    def _build_with_compiler(self, compiler: Dict) -> bool:
        """Build using a specific compiler"""
        try:
            # Create build directories
            self.detector.bin_dir.mkdir(parents=True, exist_ok=True)
            
            if compiler["type"] == "msvc":
                return self._build_with_msvc(compiler)
            elif compiler["type"] == "gcc":
                return self._build_with_gcc(compiler)
            elif compiler["type"] == "clang":
                return self._build_with_clang(compiler)
            elif compiler["type"] == "intel":
                return self._build_with_intel(compiler)
            else:
                print(f"[WARNING] Unknown compiler type: {compiler['type']}")
                return False
                
        except Exception as e:
            print(f"[ERROR] Build exception: {e}")
            return False
    
    def _build_with_msvc(self, compiler: Dict) -> bool:
        """Build with Visual Studio C++"""
        # Setup Visual Studio environment if needed
        if "setup" in compiler:
            setup_cmd = f'call "{compiler["setup"]}" && '
        else:
            setup_cmd = ""
        
        # Compile command for MSVC
        output_path = self.detector.bin_dir / "font_compiler.exe"
        compile_cmd = [
            "cl", "/EHsc", "/O2", "/DNDEBUG", "/W3",
            f"/I{self.detector.script_dir / 'include'}",
            f"/I{self.detector.script_dir / 'integration'}",
            f"/Fe:{output_path}",
            "tools/font_compiler_main.cpp"
        ]
        
        if self.detector.system == "windows":
            compile_cmd.extend(["/link", "kernel32.lib", "user32.lib", "gdi32.lib", "opengl32.lib"])
        
        full_cmd = f"{setup_cmd}{' '.join(compile_cmd)}"
        
        result = subprocess.run(full_cmd, shell=True, capture_output=True, text=True, 
                          cwd=self.detector.script_dir, timeout=60)
        
        if result.returncode == 0:
            return True
        else:
            print(f"[DEBUG] MSVC error: {result.stderr}")
            return False
    
    def _build_with_gcc(self, compiler: Dict) -> bool:
        """Build with GCC/G++"""
        compile_cmd = [
            compiler["command"],
            "-std=c++17", "-O2", "-DNDEBUG", "-Wall",
            f"-I{self.detector.script_dir / 'include'}",
            f"-I{self.detector.script_dir / 'integration'}",
            "-o", str(self.detector.bin_dir / "font_compiler"),
            "tools/font_compiler_main.cpp"
        ]
        
        # Add libraries based on platform
        if self.detector.system == "windows":
            compile_cmd.extend(["-lkernel32", "-luser32", "-lgdi32", "-lopengl32"])
        elif self.detector.system == "linux":
            compile_cmd.extend(["-lm", "-lpthread"])
        
        result = subprocess.run(compile_cmd, capture_output=True, text=True,
                          cwd=self.detector.script_dir, timeout=60)
        
        if result.returncode == 0:
            return True
        else:
            print(f"[DEBUG] GCC error: {result.stderr}")
            return False
    
    def _build_with_clang(self, compiler: Dict) -> bool:
        """Build with Clang"""
        compile_cmd = [
            compiler["command"],
            "-std=c++17", "-O2", "-DNDEBUG", "-Wall",
            f"-I{self.detector.script_dir / 'include'}",
            f"-I{self.detector.script_dir / 'integration'}",
            "-o", str(self.detector.bin_dir / "font_compiler"),
            "tools/font_compiler_main.cpp"
        ]
        
        # Add libraries based on platform
        if self.detector.system == "windows":
            compile_cmd.extend(["-lkernel32", "-luser32", "-lgdi32", "-lopengl32"])
        elif self.detector.system == "linux":
            compile_cmd.extend(["-lm", "-lpthread"])
        elif self.detector.system == "darwin":
            compile_cmd.extend(["-framework", "OpenGL", "-framework", "Cocoa"])
        
        result = subprocess.run(compile_cmd, capture_output=True, text=True,
                          cwd=self.detector.script_dir, timeout=60)
        
        if result.returncode == 0:
            return True
        else:
            print(f"[DEBUG] Clang error: {result.stderr}")
            return False
    
    def _build_with_intel(self, compiler: Dict) -> bool:
        """Build with Intel C++ Compiler"""
        compile_cmd = [
            compiler["command"],
            "-std=c++17", "-O2", "-DNDEBUG", "-Wall",
            f"-I{self.detector.script_dir / 'include'}",
            f"-I{self.detector.script_dir / 'integration'}",
            "-o", str(self.detector.bin_dir / "font_compiler"),
            "tools/font_compiler_main.cpp"
        ]
        
        result = subprocess.run(compile_cmd, capture_output=True, text=True,
                          cwd=self.detector.script_dir, timeout=60)
        
        if result.returncode == 0:
            return True
        else:
            print(f"[DEBUG] Intel compiler error: {result.stderr}")
            return False


def main():
    import argparse
    
    parser = argparse.ArgumentParser(
        description='Red Files C++ Font Compiler Builder',
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Enable verbose output')
    parser.add_argument('--list', '-l', action='store_true',
                       help='List available compilers and exit')
    parser.add_argument('--force', '-f', type=str,
                       help='Force use of specific compiler (gcc, clang, msvc, intel)')
    parser.add_argument('--clean', action='store_true',
                       help='Clean build directory before building')
    
    args = parser.parse_args()
    
    detector = CompilerDetector()
    builder = CppBuilder(detector)
    
    # Clean if requested
    if args.clean:
        import shutil
        if detector.build_dir.exists():
            print("[INFO] Cleaning build directory...")
            shutil.rmtree(detector.build_dir)
            print("[OK] Build directory cleaned")
    
    # List compilers
    if args.list:
        compilers = detector.detect_compilers()
        if compilers:
            print("Available C++ compilers:")
            for i, comp in enumerate(compilers, 1):
                print(f"  {i}. {comp['name']} v{comp['version']} ({comp['type']})")
        else:
            print("No C++ compilers found")
        return 0
    
    print("Red Files C++ Font Compiler Builder")
    print("=" * 40)
    print(f"System: {detector.system}")
    print(f"Build directory: {detector.build_dir}")
    print()
    
    # Force specific compiler if requested
    if args.force:
        compilers = detector.detect_compilers()
        target_compiler = None
        for comp in compilers:
            if comp["type"].lower() == args.force.lower():
                target_compiler = comp
                break
        
        if not target_compiler:
            print(f"[ERROR] Compiler '{args.force}' not found")
            return 1
        
        print(f"[INFO] Using forced compiler: {target_compiler['name']}")
        success = builder._build_with_compiler(target_compiler)
    else:
        # Auto-detect and build with best compiler
        success = builder.build_with_best_compiler()
    
    if success:
        print("\n" + "=" * 40)
        print("[OK] C++ font compiler built successfully!")
        print(f"Location: {detector.bin_dir / 'font_compiler'}")
        print("\nYou can now use:")
        print("  python compile.py --input font.json --output font.ttf")
        print("The script will automatically detect and use the C++ compiler")
        return 0
    else:
        print("\n[ERROR] Build failed!")
        print("Check the error messages above for details.")
        return 1


if __name__ == '__main__':
    sys.exit(main())
