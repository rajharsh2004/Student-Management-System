STUDENT MANAGEMENT SYSTEM - GUI VERSION (C++ / WINDOWS)

Features:
- Add Student (duplicate roll numbers prevented)
- Display/refresh all records
- Search by roll number
- Update student name and marks
- Delete student record (confirmation prompt)
- File handling: records are stored in students.txt
- Input validation (roll number positive; marks 0-100)

Requirements:
- Windows
- MinGW-w64 g++ compiler
- VS Code is optional

Build in VS Code / PowerShell terminal:
  g++ gui.cpp -o StudentManagementGUI.exe -mwindows

Run:
  .\StudentManagementGUI.exe

Important:
- Extract the ZIP and run commands from the extracted folder.
- students.txt is created automatically after the first student is added.
- Update: enter the existing roll number and the new name/marks in the form.
- Delete: enter roll number in the Search/Delete field (or Roll Number field).
- This is a native Windows GUI using Win32 API; it does not run on Linux/macOS unchanged.
