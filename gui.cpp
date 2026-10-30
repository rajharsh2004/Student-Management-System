#include <windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cstdio>
#include <algorithm>
#include <limits>
using namespace std;

const char *DATA_FILE = "students.txt";
HWND hRoll, hName, hMarks, hSearch, hOutput, hStatus;
HFONT hFont;

struct Student
{
    int roll;
    string name;
    float marks;
};

string getText(HWND h)
{
    int len = GetWindowTextLengthA(h);

    vector<char> buffer(len + 1, '\0');

    GetWindowTextA(h, buffer.data(), len + 1);

    return string(buffer.data());
}

void setStatus(const string &s) { SetWindowTextA(hStatus, s.c_str()); }

vector<Student> loadStudents()
{
    vector<Student> students;
    ifstream f(DATA_FILE);
    Student s;
    string roll, name, marks;
    while (getline(f, roll) && getline(f, name) && getline(f, marks))
    {
        try
        {
            s.roll = stoi(roll);
            s.name = name;
            s.marks = stof(marks);
        }
        catch (...)
        {
            continue;
        }
        students.push_back(s);
    }
    return students;
}
bool saveStudents(const vector<Student> &students)
{
    ofstream f(DATA_FILE, ios::trunc);
    if (!f)
        return false;
    for (const auto &s : students)
        f << s.roll << '\n'
          << s.name << '\n'
          << fixed << setprecision(2) << s.marks << '\n';
    return true;
}
void refreshOutput(const vector<Student> &students)
{
    ostringstream out;
    out << left << setw(10) << "Roll No." << setw(28) << "Student Name" << "Marks (%)\r\n";
    out << string(55, '-') << "\r\n";
    for (const auto &s : students)
        out << left << setw(10) << s.roll << setw(28) << s.name
            << fixed << setprecision(2) << s.marks << "\r\n";
    if (students.empty())
        out << "No student records found.\r\n";
    SetWindowTextA(hOutput, out.str().c_str());
}
bool readForm(Student &s)
{
    string roll = getText(hRoll), name = getText(hName), marks = getText(hMarks);
    if (roll.empty() || name.empty() || marks.empty())
    {
        MessageBoxA(NULL, "Please fill in Roll Number, Student Name, and Marks.", "Missing Input", MB_OK | MB_ICONWARNING);
        return false;
    }
    try
    {
        s.roll = stoi(roll);
        s.marks = stof(marks);
    }
    catch (...)
    {
        MessageBoxA(NULL, "Roll number and marks must be numeric.", "Invalid Input", MB_OK | MB_ICONWARNING);
        return false;
    }
    if (s.roll <= 0 || s.marks < 0 || s.marks > 100)
    {
        MessageBoxA(NULL, "Roll number must be positive and marks must be between 0 and 100.", "Invalid Input", MB_OK | MB_ICONWARNING);
        return false;
    }
    s.name = name;
    return true;
}
void clearForm()
{
    SetWindowTextA(hRoll, "");
    SetWindowTextA(hName, "");
    SetWindowTextA(hMarks, "");
    SetWindowTextA(hSearch, "");
}
void addStudent()
{
    Student s;
    if (!readForm(s))
        return;
    auto v = loadStudents();
    for (const auto &x : v)
        if (x.roll == s.roll)
        {
            MessageBoxA(NULL, "That roll number already exists.", "Duplicate Record", MB_OK | MB_ICONWARNING);
            return;
        }
    v.push_back(s);
    if (!saveStudents(v))
    {
        setStatus("Could not save records.");
        return;
    }
    refreshOutput(v);
    setStatus("Student added successfully.");
    clearForm();
}
void searchStudent()
{
    string val = getText(hSearch);
    if (val.empty())
    {
        MessageBoxA(NULL, "Enter a roll number in the Search/Delete field.", "Search", MB_OK | MB_ICONWARNING);
        return;
    }
    int roll;
    try
    {
        roll = stoi(val);
    }
    catch (...)
    {
        MessageBoxA(NULL, "Roll number must be numeric.", "Invalid Input", MB_OK | MB_ICONWARNING);
        return;
    }
    for (const auto &s : loadStudents())
        if (s.roll == roll)
        {
            ostringstream out;
            out << "Student Found\n\nRoll Number: " << s.roll << "\nName: " << s.name
                << "\nMarks: " << fixed << setprecision(2) << s.marks << "%";
            MessageBoxA(NULL, out.str().c_str(), "Search Result", MB_OK | MB_ICONINFORMATION);
            setStatus("Student found.");
            return;
        }
    MessageBoxA(NULL, "Student not found.", "Search Result", MB_OK | MB_ICONINFORMATION);
    setStatus("No matching student found.");
}
void updateStudent()
{
    Student updated;
    if (!readForm(updated))
        return;
    auto v = loadStudents();
    bool found = false;
    for (auto &s : v)
        if (s.roll == updated.roll)
        {
            s.name = updated.name;
            s.marks = updated.marks;
            found = true;
            break;
        }
    if (!found)
    {
        MessageBoxA(NULL, "Roll number not found. Enter the existing roll number and updated details.", "Update", MB_OK | MB_ICONWARNING);
        return;
    }
    if (!saveStudents(v))
    {
        setStatus("Could not save updated records.");
        return;
    }
    refreshOutput(v);
    setStatus("Student updated successfully.");
    clearForm();
}
void deleteStudent()
{
    string val = getText(hSearch);
    if (val.empty())
        val = getText(hRoll);
    if (val.empty())
    {
        MessageBoxA(NULL, "Enter a roll number in Search/Delete or Roll Number.", "Delete", MB_OK | MB_ICONWARNING);
        return;
    }
    int roll;
    try
    {
        roll = stoi(val);
    }
    catch (...)
    {
        MessageBoxA(NULL, "Roll number must be numeric.", "Invalid Input", MB_OK | MB_ICONWARNING);
        return;
    }
    if (MessageBoxA(NULL, "Delete this student record?", "Confirm Delete", MB_YESNO | MB_ICONQUESTION) != IDYES)
        return;
    auto v = loadStudents();
    size_t before = v.size();
    v.erase(remove_if(v.begin(), v.end(), [roll](const Student &s)
                      { return s.roll == roll; }),
            v.end());
    if (v.size() == before)
    {
        MessageBoxA(NULL, "Student not found.", "Delete", MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (!saveStudents(v))
    {
        setStatus("Could not save records after deletion.");
        return;
    }
    refreshOutput(v);
    setStatus("Student deleted successfully.");
    clearForm();
}
void addControl(HWND parent, const char *cls, const char *text, DWORD style,
                int x, int y, int w, int h, int id)
{
    HWND c = CreateWindowExA(0, cls, text, WS_CHILD | WS_VISIBLE | style,
                             x, y, w, h, parent, (HMENU)(INT_PTR)id, GetModuleHandle(NULL), NULL);
    if (hFont)
        SendMessageA(c, WM_SETFONT, (WPARAM)hFont, TRUE);
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CREATE:
        hFont = CreateFontA(17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");
        addControl(hwnd, "STATIC", "STUDENT MANAGEMENT SYSTEM", SS_CENTER, 20, 15, 740, 35, 100);
        addControl(hwnd, "STATIC", "Roll Number:", 0, 25, 65, 100, 25, 101);
        addControl(hwnd, "EDIT", "", WS_BORDER | ES_AUTOHSCROLL, 135, 62, 170, 28, 102);
        hRoll = GetDlgItem(hwnd, 102);
        addControl(hwnd, "STATIC", "Student Name:", 0, 25, 105, 100, 25, 103);
        addControl(hwnd, "EDIT", "", WS_BORDER | ES_AUTOHSCROLL, 135, 102, 250, 28, 104);
        hName = GetDlgItem(hwnd, 104);
        addControl(hwnd, "STATIC", "Marks (0-100):", 0, 25, 145, 100, 25, 105);
        addControl(hwnd, "EDIT", "", WS_BORDER | ES_AUTOHSCROLL, 135, 142, 170, 28, 106);
        hMarks = GetDlgItem(hwnd, 106);
        addControl(hwnd, "BUTTON", "Add Student", BS_PUSHBUTTON, 25, 185, 125, 35, 201);
        addControl(hwnd, "BUTTON", "Update Student", BS_PUSHBUTTON, 160, 185, 135, 35, 202);
        addControl(hwnd, "BUTTON", "Clear Form", BS_PUSHBUTTON, 305, 185, 105, 35, 203);
        addControl(hwnd, "BUTTON", "Show All", BS_PUSHBUTTON, 420, 185, 100, 35, 204);
        addControl(hwnd, "STATIC", "Search/Delete Roll No.:", 0, 25, 238, 165, 25, 107);
        addControl(hwnd, "EDIT", "", WS_BORDER | ES_AUTOHSCROLL, 190, 235, 140, 28, 108);
        hSearch = GetDlgItem(hwnd, 108);
        addControl(hwnd, "BUTTON", "Search", BS_PUSHBUTTON, 340, 232, 90, 35, 205);
        addControl(hwnd, "BUTTON", "Delete", BS_PUSHBUTTON, 440, 232, 90, 35, 206);
        addControl(hwnd, "STATIC", "Student Records", 0, 25, 280, 200, 25, 109);
        addControl(hwnd, "EDIT", "", WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_READONLY,
                   25, 310, 730, 250, 110);
        hOutput = GetDlgItem(hwnd, 110);
        addControl(hwnd, "STATIC", "Ready.", 0, 25, 570, 730, 25, 111);
        hStatus = GetDlgItem(hwnd, 111);
        refreshOutput(loadStudents());
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wp))
        {
        case 201:
            addStudent();
            return 0;
        case 202:
            updateStudent();
            return 0;
        case 203:
            clearForm();
            setStatus("Form cleared.");
            return 0;
        case 204:
            refreshOutput(loadStudents());
            setStatus("Records refreshed.");
            return 0;
        case 205:
            searchStudent();
            return 0;
        case 206:
            deleteStudent();
            return 0;
        }
        break;
    case WM_DESTROY:
        if (hFont)
            DeleteObject(hFont);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}
int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int show)
{
    const char CLASS_NAME[] = "StudentManagementSystemGUI";
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    if (!RegisterClassA(&wc))
        return 1;
    HWND hwnd = CreateWindowExA(0, CLASS_NAME, "Student Management System",
                                WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 650,
                                NULL, NULL, instance, NULL);
    if (!hwnd)
        return 1;
    ShowWindow(hwnd, show);
    UpdateWindow(hwnd);
    MSG msg = {};
    while (GetMessageA(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return (int)msg.wParam;
}
