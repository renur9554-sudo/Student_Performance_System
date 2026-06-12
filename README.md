# Student Performance Analysis System

A comprehensive web-based system for managing student records, tracking attendance, analyzing performance, and generating reports.

## 🚀 Features

### Authentication
- ✅ Teacher Login & Signup with secure password hashing
- ✅ Session management

### Student Management
1. **Add Student** - Register new students with roll number, name, semester, and marks
2. **Display All Students** - View complete list of registered students
3. **Add Extracurricular Activity** - Track student activities beyond academics
4. **Display Extracurricular Activities** - View all activities by students
5. **Display Sports Activities** - See sports participation records

### Performance Analysis
6. **Find Lowest Marks (Individual)** - Analyze weak subjects for specific students
7. **Find Lowest Marks (All Students)** - Class-wide performance analysis
8. **Compare Student Semesters** - Detailed performance comparison with grades

### Attendance Management
9. **Mark Attendance** - Record daily attendance for individual students
10. **Mark Bulk Attendance** - Mark attendance for multiple students at once
11. **View Student Attendance** - Individual attendance records and statistics
12. **View All Attendance** - Class-wide attendance overview
13. **Low Attendance Alerts** - Identify students below attendance threshold
14. **Monthly Attendance Report** - Generate month-wise attendance reports
15. **Undo Last Attendance** - Revert incorrect attendance entries

## 📦 Installation

### Prerequisites
- Python 3.7 or higher
- pip (Python package manager)

### Step 1: Setup Project Structure

Create the following folder structure:

```
student-performance-system/
│
├── app.py                  # Flask backend
├── requirements.txt        # Python dependencies
├── students.json          # Auto-generated data file
├── teachers.json          # Auto-generated data file
│
├── templates/
│   └── index.html         # Frontend HTML file
│
└── static/                # Optional: for CSS/JS files
    ├── css/
    └── js/
```

### Step 2: Install Dependencies

Open terminal/command prompt in your project folder and run:

```bash
pip install -r requirements.txt
```

Or install manually:

```bash
pip install Flask==2.3.0 Flask-CORS==4.0.0 python-dotenv==1.0.0
```

### Step 3: Add Frontend File

1. Create a folder named `templates` in your project directory
2. Copy the provided `index.html` file into the `templates` folder

### Step 4: Run the Application

Start the Flask server:

```bash
python app.py
```

You should see output like:
```
🚀 Starting Flask Server...
📍 Server URL: http://127.0.0.1:5000
🌐 Access your app at: http://127.0.0.1:5000
⚡ Press CTRL+C to stop the server
```

### Step 5: Access the Application

Open your web browser and navigate to:
```
http://127.0.0.1:5000
```

## 🎯 Usage

### First Time Setup
1. Click "Create New Account" on the login page
2. Fill in your details (username, email, password)
3. Click "Sign Up"
4. Login with your credentials

### Adding Students
1. Navigate to "Add Student" from the dashboard
2. Enter student details:
   - Roll Number
   - Name
   - Semester
   - Marks for 5 subjects
3. Click "Add Student"

### Marking Attendance
1. Select "Mark Attendance" from the menu
2. Enter student roll number
3. Select date
4. Choose status: Present (P), Absent (A), Late (L), or Half-day (H)
5. Submit

### Viewing Reports
- **Low Attendance Alerts**: Set threshold percentage to see students below that limit
- **Monthly Report**: Select month (YYYY-MM format) to generate report

## 🔧 Configuration

### Changing Port
Edit `app.py` and modify the last line:
```python
app.run(debug=True, host='0.0.0.0', port=5001)  # Change 5000 to any port
```

### Security Settings
Change the secret key in `app.py`:
```python
app.secret_key = 'your-unique-secret-key-here'
```

## 📊 Data Storage

The system uses JSON files for data storage:
- `students.json` - Stores all student records
- `teachers.json` - Stores teacher accounts

These files are automatically created when you run the application.

## 🐛 Troubleshooting

### Error: "Module not found"
**Solution:** Install missing packages
```bash
pip install flask flask-cors
```

### Error: "Port already in use"
**Solution:** Change the port number in app.py or kill the process using that port

### Error: "Template not found"
**Solution:** Ensure `index.html` is inside the `templates/` folder

### Error: "Connection refused"
**Solution:** Make sure the Flask server is running (`python app.py`)

### Login/Signup Not Working
**Solution:** 
1. Check browser console for errors (F12)
2. Verify Flask server is running
3. Ensure no CORS errors in console

## 🔒 Security Notes

- Passwords are hashed using SHA-256 before storage
- Session management for authenticated users
- Input validation on both frontend and backend
- CORS enabled for API access

## 📝 API Endpoints

### Authentication
- `POST /api/signup` - Create new teacher account
- `POST /api/login` - Login for teachers

### Student Management
- `POST /api/student` - Add new student
- `GET /api/students` - Get all students

### Activities
- `POST /api/activity` - Add activity for student
- `GET /api/activities` - Get all activities
- `GET /api/sports` - Get all sports activities

### Performance Analysis
- `GET /api/lowest-marks?type=individual&rollNo=X` - Get lowest marks for student
- `GET /api/lowest-marks?type=all` - Get lowest marks for all students
- `GET /api/compare?rollNo=X` - Compare student performance

### Attendance
- `POST /api/attendance` - Mark single attendance
- `POST /api/bulk-attendance` - Mark multiple attendance
- `GET /api/attendance-view?type=individual&rollNo=X` - View individual attendance
- `GET /api/attendance-view?type=all` - View all attendance

### Reports
- `GET /api/report?type=low&threshold=75` - Low attendance alert
- `GET /api/report?type=monthly&month=2025-11` - Monthly report

## 💻 Technology Stack

- **Frontend:** HTML5, CSS3, JavaScript (Vanilla)
- **Backend:** Python Flask
- **Data Storage:** JSON files
- **Authentication:** Session-based with password hashing

## 📞 Support

If you encounter any issues:
1. Check the troubleshooting section above
2. Verify all files are in correct folders
3. Ensure all dependencies are installed
4. Check Python version (3.7+)

## 🎉 Success!

Once everything is set up, you should have a fully functional Student Performance Analysis System with:
- ✅ Beautiful responsive frontend
- ✅ Complete Flask backend
- ✅ Secure authentication
- ✅ All 15 features working
- ✅ No errors!

---

**Note:** This is a development setup. For production use, consider:
- Using a proper database (PostgreSQL/MySQL)
- Implementing proper authentication (JWT tokens)
- Adding HTTPS support
- Using environment variables for sensitive data
- Adding proper logging and error handling
