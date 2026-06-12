from flask import Flask, render_template, request, redirect, url_for, session, flash
import json
import os
from datetime import datetime, timedelta
import random
import calendar

app = Flask(__name__)
app.secret_key = 'your-secret-key-change-in-production-12345'

# File paths
BASE_DIR = 'data'
USERS_FILE = 'users.json'

# Default credentials
DEFAULT_USERNAME = 'teacher'
DEFAULT_PASSWORD = 'teacher123'

# Sample data
INDIAN_NAMES = [
    "Aarav Sharma", "Vivaan Patel", "Aditya Kumar", "Vihaan Singh",
    "Arjun Gupta", "Sai Reddy", "Arnav Mehta", "Ayaan Khan",
    "Krishna Verma", "Ishaan Joshi", "Shaurya Nair", "Atharv Desai",
    "Advik Iyer", "Pranav Rao", "Reyansh Pandey", "Aadhya Sharma",
    "Ananya Patel", "Diya Kumar", "Saanvi Singh", "Isha Gupta",
    "Navya Reddy", "Kiara Mehta", "Aanya Khan", "Myra Verma",
    "Sara Joshi", "Pari Nair", "Riya Desai", "Avni Iyer",
    "Anaya Rao", "Shanaya Pandey"
]

SPORTS = ["Cricket", "Football", "Basketball", "Badminton", "Table Tennis",
          "Chess", "Swimming", "Athletics", "Volleyball", "Tennis", None, None]

ACTIVITIES = ["Drama Club", "Debate Society", "Music Band", "Art Club",
              "Coding Club", "Dance Team", "Photography", "Science Club",
              "Literature Society", "Robotics Club", None, None]

# ═══════════════════════════════════════════════════════════
# HELPER FUNCTIONS
# ═══════════════════════════════════════════════════════════

def get_user_dir(username):
    """Get user-specific data directory"""
    user_dir = os.path.join(BASE_DIR, username)
    if not os.path.exists(user_dir):
        os.makedirs(user_dir)
    return user_dir

def get_user_file(username, filename):
    """Get path to user-specific file"""
    return os.path.join(get_user_dir(username), filename)

def load_json(filepath):
    """Load data from JSON file"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            return json.load(f)
    except FileNotFoundError:
        return [] if 'users' not in filepath else {}

def save_json(filepath, data):
    """Save data to JSON file"""
    with open(filepath, 'w', encoding='utf-8') as f:
        json.dump(data, f, indent=2, ensure_ascii=False)

def generate_sample_data_for_default():
    """Generate 30 students ONLY for default teacher"""
    students = []
    attendance = []
    start_roll = 2418491
    start_date = datetime.now() - timedelta(days=60)
    
    print("\n🎯 Generating sample data for default teacher...")
    
    for i in range(30):
        student = {
            "rollNo": start_roll + i,
            "name": INDIAN_NAMES[i],
            "semester": random.randint(1, 8),
            "marks": [random.randint(45, 98) for _ in range(5)],
            "sports": random.choice(SPORTS),
            "activities": []
        }
        
        num_activities = random.randint(0, 3)
        if num_activities > 0:
            student["activities"] = random.sample([a for a in ACTIVITIES if a], num_activities)
        
        students.append(student)
        
        num_records = random.randint(40, 50)
        for j in range(num_records):
            date = start_date + timedelta(days=j)
            rand = random.random()
            
            if rand < 0.80:
                status = "P"
            elif rand < 0.90:
                status = "A"
            elif rand < 0.95:
                status = "L"
            else:
                status = "H"
            
            attendance.append({
                "rollNo": student["rollNo"],
                "date": date.strftime("%Y-%m-%d"),
                "status": status
            })
    
    save_json(get_user_file(DEFAULT_USERNAME, 'students.json'), students)
    save_json(get_user_file(DEFAULT_USERNAME, 'attendance.json'), attendance)
    
    print(f"   ✅ {len(students)} students")
    print(f"   ✅ {len(attendance)} attendance records")

def create_empty_data_for_new_user(username):
    """Create EMPTY data files for new teacher"""
    save_json(get_user_file(username, 'students.json'), [])
    save_json(get_user_file(username, 'attendance.json'), [])
    print(f"✅ Created empty data for new teacher: {username}")

def initialize_system():
    """Initialize system"""
    if not os.path.exists(BASE_DIR):
        os.makedirs(BASE_DIR)
    
    # Create users file if not exists
    if not os.path.exists(USERS_FILE):
        users = {
            DEFAULT_USERNAME: {"password": DEFAULT_PASSWORD, "role": "teacher"}
        }
        save_json(USERS_FILE, users)
        print(f"✅ Created users.json with default user")
        
        # Generate sample data ONLY for default teacher
        generate_sample_data_for_default()
    else:
        # Check if default teacher data exists, if not generate it
        default_students_file = get_user_file(DEFAULT_USERNAME, 'students.json')
        if not os.path.exists(default_students_file):
            print("⚠️  Default teacher data missing, regenerating...")
            generate_sample_data_for_default()

def calculate_attendance_percentage(username, roll_no):
    """Calculate attendance percentage"""
    attendance_file = get_user_file(username, 'attendance.json')
    attendance = load_json(attendance_file)
    student_records = [r for r in attendance if r['rollNo'] == roll_no]
    
    if not student_records:
        return 0.0
    
    present = sum(1 for r in student_records if r['status'] == 'P')
    total = len(student_records)
    
    return (present / total * 100) if total > 0 else 0.0

# ═══════════════════════════════════════════════════════════
# ROUTES
# ═══════════════════════════════════════════════════════════

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        username = request.form.get('username')
        password = request.form.get('password')
        
        users = load_json(USERS_FILE)
        
        if username in users and users[username]['password'] == password:
            session['username'] = username
            session['role'] = users[username]['role']
            flash('Login successful!', 'success')
            return redirect(url_for('dashboard'))
        else:
            flash('Invalid username or password!', 'error')
    
    return render_template('login.html')

@app.route('/signup', methods=['GET', 'POST'])
def signup():
    if request.method == 'POST':
        username = request.form.get('username')
        password = request.form.get('password')
        confirm_password = request.form.get('confirm_password')
        
        # Validation
        if len(username) < 3:
            flash('Username must be at least 3 characters!', 'error')
            return render_template('signup.html')
        
        if len(password) < 6:
            flash('Password must be at least 6 characters!', 'error')
            return render_template('signup.html')
        
        if password != confirm_password:
            flash('Passwords do not match!', 'error')
            return render_template('signup.html')
        
        users = load_json(USERS_FILE)
        
        if username in users:
            flash('Username already exists!', 'error')
            return render_template('signup.html')
        
        # Create new user
        users[username] = {'password': password, 'role': 'teacher'}
        save_json(USERS_FILE, users)
        
        # Create EMPTY data for new teacher
        create_empty_data_for_new_user(username)
        
        flash('Account created successfully! Please login.', 'success')
        return redirect(url_for('login'))
    
    return render_template('signup.html')

@app.route('/dashboard')
def dashboard():
    if 'username' not in session:
        return redirect(url_for('login'))
    return render_template('dashboard.html')

@app.route('/logout')
def logout():
    session.clear()
    flash('Logged out successfully!', 'success')
    return redirect(url_for('index'))

# ═══════════════════════════════════════════════════════════
# STUDENT ROUTES
# ═══════════════════════════════════════════════════════════

@app.route('/add_student', methods=['GET', 'POST'])
def add_student():
    if 'username' not in session:
        return redirect(url_for('login'))
    
    username = session['username']
    
    if request.method == 'POST':
        students_file = get_user_file(username, 'students.json')
        students = load_json(students_file)
        
        new_student = {
            'rollNo': int(request.form.get('rollNo')),
            'name': request.form.get('name'),
            'semester': int(request.form.get('semester')),
            'marks': [
                int(request.form.get('marks1')),
                int(request.form.get('marks2')),
                int(request.form.get('marks3')),
                int(request.form.get('marks4')),
                int(request.form.get('marks5'))
            ],
            'sports': request.form.get('sports') or None,
            'activities': []
        }
        
        students.append(new_student)
        save_json(students_file, students)
        flash('Student added successfully!', 'success')
        return redirect(url_for('add_student'))
    
    return render_template('add-student.html')

@app.route('/display_students')
def display_students():
    if 'username' not in session:
        return redirect(url_for('login'))
    
    username = session['username']
    students_file = get_user_file(username, 'students.json')
    students = load_json(students_file)
    
    for student in students:
        student['attendancePercentage'] = calculate_attendance_percentage(username, student['rollNo'])
    
    return render_template('display-students.html', students=students)

# ═══════════════════════════════════════════════════════════
# ATTENDANCE ROUTES
# ═══════════════════════════════════════════════════════════

@app.route('/mark_attendance', methods=['GET', 'POST'])
def mark_attendance():
    if 'username' not in session:
        return redirect(url_for('login'))
    
    username = session['username']
    
    if request.method == 'POST':
        attendance_file = get_user_file(username, 'attendance.json')
        attendance = load_json(attendance_file)
        
        new_record = {
            'rollNo': int(request.form.get('rollNo')),
            'date': request.form.get('date'),
            'status': request.form.get('status')
        }
        
        attendance.append(new_record)
        save_json(attendance_file, attendance)
        flash('Attendance marked successfully!', 'success')
        return redirect(url_for('mark_attendance'))
    
    return render_template('mark-attendance.html')

@app.route('/view_all_attendance')
def view_all_attendance():
    if 'username' not in session:
        return redirect(url_for('login'))
    
    username = session['username']
    students_file = get_user_file(username, 'students.json')
    attendance_file = get_user_file(username, 'attendance.json')
    
    students = load_json(students_file)
    attendance = load_json(attendance_file)
    
    attendance_data = []
    for student in students:
        records = [r for r in attendance if r['rollNo'] == student['rollNo']]
        
        data = {
            'rollNo': student['rollNo'],
            'name': student['name'],
            'totalPresent': sum(1 for r in records if r['status'] == 'P'),
            'totalAbsent': sum(1 for r in records if r['status'] == 'A'),
            'totalLate': sum(1 for r in records if r['status'] == 'L'),
            'totalHalfDay': sum(1 for r in records if r['status'] == 'H'),
            'attendancePercentage': calculate_attendance_percentage(username, student['rollNo'])
        }
        
        attendance_data.append(data)
    
    return render_template('view-all-attendance.html', attendance_data=attendance_data)

@app.route('/view_student_attendance', methods=['GET'])
def view_student_attendance():
    if 'username' not in session:
        return redirect(url_for('login'))
    
    username = session['username']
    roll_no = request.args.get('rollNo', type=int)
    data = None
    
    if roll_no:
        students_file = get_user_file(username, 'students.json')
        attendance_file = get_user_file(username, 'attendance.json')
        
        students = load_json(students_file)
        attendance = load_json(attendance_file)
        
        student = next((s for s in students if s['rollNo'] == roll_no), None)
        
        if student:
            records = [r for r in attendance if r['rollNo'] == roll_no]
            
            data = {
                'rollNo': student['rollNo'],
                'name': student['name'],
                'totalPresent': sum(1 for r in records if r['status'] == 'P'),
                'totalAbsent': sum(1 for r in records if r['status'] == 'A'),
                'totalLate': sum(1 for r in records if r['status'] == 'L'),
                'totalHalfDay': sum(1 for r in records if r['status'] == 'H'),
                'attendancePercentage': calculate_attendance_percentage(username, roll_no),
                'records': sorted(records, key=lambda x: x['date'], reverse=True)
            }
    
    return render_template('view-student-attendance.html', data=data)

@app.route('/low_attendance_alerts', methods=['GET'])
def low_attendance_alerts():
    if 'username' not in session:
        return redirect(url_for('login'))
    
    username = session['username']
    threshold = request.args.get('threshold', 75, type=float)
    
    students_file = get_user_file(username, 'students.json')
    attendance_file = get_user_file(username, 'attendance.json')
    
    students = load_json(students_file)
    attendance = load_json(attendance_file)
    
    low_attendance = []
    for student in students:
        records = [r for r in attendance if r['rollNo'] == student['rollNo']]
        percentage = calculate_attendance_percentage(username, student['rollNo'])
        
        if percentage < threshold:
            low_attendance.append({
                'rollNo': student['rollNo'],
                'name': student['name'],
                'attendancePercentage': percentage,
                'totalPresent': sum(1 for r in records if r['status'] == 'P'),
                'totalAbsent': sum(1 for r in records if r['status'] == 'A')
            })
    
    return render_template('low-attendance-alerts.html', data=low_attendance)

# ═══════════════════════════════════════════════════════════
# BULK ATTENDANCE ROUTE (FIXED)
# ═══════════════════════════════════════════════════════════

@app.route('/bulk_attendance', methods=['GET', 'POST'])
def bulk_attendance():
    if 'username' not in session:
        return redirect(url_for('login'))
    
    username = session['username']
    
    if request.method == 'POST':
        date = request.form.get('date')
        status = request.form.get('status')
        selected_students = request.form.getlist('students')
        
        if not selected_students:
            flash('Please select at least one student!', 'error')
            students_file = get_user_file(username, 'students.json')
            students = load_json(students_file)
            return render_template('bulk-attendance.html', students=students)
        
        attendance_file = get_user_file(username, 'attendance.json')
        attendance = load_json(attendance_file)
        
        # Mark attendance for all selected students
        for roll_no in selected_students:
            attendance.append({
                'rollNo': int(roll_no),
                'date': date,
                'status': status
            })
        
        save_json(attendance_file, attendance)
        flash(f'Attendance marked for {len(selected_students)} students!', 'success')
        return redirect(url_for('bulk_attendance'))
    
    # Load students for display
    students_file = get_user_file(username, 'students.json')
    students = load_json(students_file)
    
    return render_template('bulk-attendance.html', students=students)

# ═══════════════════════════════════════════════════════════
# MONTHLY REPORT ROUTE (FIXED)
# ═══════════════════════════════════════════════════════════

@app.route('/monthly_report', methods=['GET'])
def monthly_report():
    if 'username' not in session:
        return redirect(url_for('login'))

    username = session['username']
    month = request.args.get('month', type=int)
    year = request.args.get('year', type=int)

    data = None

    if month and year:
        students_file = get_user_file(username, 'students.json')
        attendance_file = get_user_file(username, 'attendance.json')

        students = load_json(students_file)
        attendance = load_json(attendance_file)

        # Month filter
        month_start = f"{year}-{month:02d}-01"
        month_end_day = calendar.monthrange(year, month)[1]
        month_end = f"{year}-{month:02d}-{month_end_day}"

        month_attendance = [
            r for r in attendance
            if month_start <= r['date'] <= month_end
        ]

        student_stats = []
        total_present = 0
        total_absent = 0

        for student in students:
            student_records = [
                r for r in month_attendance
                if r['rollNo'] == student['rollNo']
            ]

            present = sum(1 for r in student_records if r['status'] == 'P')
            absent = sum(1 for r in student_records if r['status'] == 'A')
            late = sum(1 for r in student_records if r['status'] == 'L')
            total = len(student_records)

            percentage = (present / total) * 100 if total > 0 else 0

            total_present += present
            total_absent += absent

            student_stats.append({
                'rollNo': student['rollNo'],
                'name': student['name'],
                'present': present,
                'absent': absent,
                'late': late,
                'percentage': round(percentage, 1)
            })

        total_records = len(month_attendance)

        avg_present = (total_present / total_records * 100) if total_records > 0 else 0
        avg_absent = (total_absent / total_records * 100) if total_records > 0 else 0

        month_name = calendar.month_name[month]

        data = {
            'month': month_name,
            'year': year,
            'totalStudents': len(students),
            'avgPresent': round(avg_present, 1),
            'avgAbsent': round(avg_absent, 1),
            'students': student_stats
        }

    return render_template('monthly-report.html', data=data)
# ═══════════════════════════════════════════════════════════
# ACTIVITY ROUTES
# ═══════════════════════════════════════════════════════════

@app.route('/add_activity', methods=['GET', 'POST'])
def add_activity():
    if 'username' not in session:
        return redirect(url_for('login'))
    
    username = session['username']
    
    if request.method == 'POST':
        students_file = get_user_file(username, 'students.json')
        students = load_json(students_file)
        roll_no = int(request.form.get('rollNo'))
        activity = request.form.get('activity')
        
        for student in students:
            if student['rollNo'] == roll_no:
                if 'activities' not in student:
                    student['activities'] = []
                student['activities'].append(activity)
                break
        
        save_json(students_file, students)
        flash('Activity added successfully!', 'success')
        return redirect(url_for('add_activity'))
    
    return render_template('add-activity.html')

@app.route('/display_activities')
def display_activities():
    if 'username' not in session:
        return redirect(url_for('login'))
    
    username = session['username']
    students_file = get_user_file(username, 'students.json')
    students = load_json(students_file)
    activities = [s for s in students if s.get('activities')]
    
    return render_template('display-activities.html', activities=activities)

@app.route('/display_sports')
def display_sports():
    if 'username' not in session:
        return redirect(url_for('login'))
    
    username = session['username']
    students_file = get_user_file(username, 'students.json')
    students = load_json(students_file)
    sports = [s for s in students if s.get('sports')]
    
    return render_template('display-sports.html', sports=sports)

# ═══════════════════════════════════════════════════════════
# MARKS ANALYSIS ROUTES
# ═══════════════════════════════════════════════════════════

@app.route('/lowest_marks_individual', methods=['GET'])
def lowest_marks_individual():
    if 'username' not in session:
        return redirect(url_for('login'))
    
    username = session['username']
    roll_no = request.args.get('rollNo', type=int)
    data = None
    
    if roll_no:
        students_file = get_user_file(username, 'students.json')
        students = load_json(students_file)
        student = next((s for s in students if s['rollNo'] == roll_no), None)
        
        if student:
            marks = student['marks']
            avg = sum(marks) / len(marks)
            lowest = sorted(enumerate(marks, 1), key=lambda x: x[1])[:2]
            
            data = {
                'rollNo': student['rollNo'],
                'name': student['name'],
                'average': avg,
                'lowestMarks': [{'subject': i, 'marks': m} for i, m in lowest]
            }
    
    return render_template('lowest-marks-individual.html', data=data)

@app.route('/lowest_marks_all')
def lowest_marks_all():
    if 'username' not in session:
        return redirect(url_for('login'))
    
    username = session['username']
    students_file = get_user_file(username, 'students.json')
    students = load_json(students_file)
    data = []
    
    for student in students:
        marks = student['marks']
        avg = sum(marks) / len(marks)
        lowest = sorted(enumerate(marks, 1), key=lambda x: x[1])[:2]
        
        data.append({
            'rollNo': student['rollNo'],
            'name': student['name'],
            'average': avg,
            'lowestMarks': [{'subject': i, 'marks': m} for i, m in lowest]
        })
    
    return render_template('lowest-marks-all.html', data=data)

@app.route('/compare_student', methods=['GET'])
def compare_student():
    if 'username' not in session:
        return redirect(url_for('login'))
    
    username = session['username']
    roll_no = request.args.get('rollNo', type=int)
    data = None
    
    if roll_no:
        students_file = get_user_file(username, 'students.json')
        students = load_json(students_file)
        student = next((s for s in students if s['rollNo'] == roll_no), None)
        
        if student:
            marks = student['marks']
            avg = sum(marks) / len(marks)
            
            if avg >= 90:
                performance = "Excellent"
            elif avg >= 75:
                performance = "Good"
            elif avg >= 60:
                performance = "Average"
            else:
                performance = "Needs Improvement"
            
            data = {
                'rollNo': student['rollNo'],
                'name': student['name'],
                'semester': student['semester'],
                'average': avg,
                'performance': performance,
                'attendancePercentage': calculate_attendance_percentage(username, roll_no),
                'subjectPerformance': [
                    {'subject': i+1, 'marks': m, 'grade': 'A' if m >= 90 else 'B' if m >= 75 else 'C' if m >= 60 else 'D'}
                    for i, m in enumerate(marks)
                ]
            }
    
    return render_template('compare-student.html', data=data)

# ═══════════════════════════════════════════════════════════
# ADDITIONAL ROUTES
# ═══════════════════════════════════════════════════════════

@app.route('/undo_attendance', methods=['GET', 'POST'])
def undo_attendance():
    if 'username' not in session:
        return redirect(url_for('login'))
    return render_template('undo-attendance.html')

# ═══════════════════════════════════════════════════════════
# APP INITIALIZATION
# ═══════════════════════════════════════════════════════════

if __name__ == '__main__':
    print("\n" + "="*60)
    print("🚀 STUDENT PERFORMANCE ANALYSIS SYSTEM")
    print("="*60)
    
    initialize_system()
    
    print("\n🔑 Login Options:")
    print(f"   Default Teacher (with 30 students):")
    print(f"     Username: {DEFAULT_USERNAME}")
    print(f"     Password: {DEFAULT_PASSWORD}")
    print(f"\n   New Teachers (empty data - add students yourself):")
    print(f"     Create account at: http://127.0.0.1:5000/signup")
    
    print("\n✅ Features:")
    print("   - Each teacher gets isolated data")
    print("   - Default teacher has 30 sample students")
    print("   - New teachers start with empty data")
    print("   - Login/Signup error messages working")
    print("   - Password confirmation on signup")
    print("   - Bulk attendance working")
    print("   - Monthly reports working")
    
    print("\n🌐 Starting server...")
    print("   URL: http://127.0.0.1:5000")
    print("="*60 + "\n")
    
    app.run(debug=True)
