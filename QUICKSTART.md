# Quick Setup Guide

## 🚀 Get Started in 3 Minutes!

### Step 1: Create Project Folder
```bash
mkdir student-performance-system
cd student-performance-system
```

### Step 2: Create Folder Structure
```bash
mkdir templates
mkdir static
```

Your folder should look like:
```
student-performance-system/
├── templates/
└── static/
```

### Step 3: Add Files
1. Copy `app.py` to the main folder
2. Copy `requirements.txt` to the main folder
3. Copy `index.html` to the `templates/` folder

### Step 4: Install Dependencies
```bash
pip install -r requirements.txt
```

### Step 5: Run the Server
```bash
python app.py
```

### Step 6: Open Browser
Go to: **http://127.0.0.1:5000**

---

## ✅ Verify Installation

You should see:
- ✅ Login page loads
- ✅ Can click "Create New Account"
- ✅ No console errors (press F12)

---

## 🎯 First Steps

1. **Sign Up**: Create teacher account
2. **Login**: Use your credentials
3. **Add Student**: Test by adding a student
4. **Mark Attendance**: Try marking attendance
5. **View Reports**: Check attendance reports

---

## 🐛 Common Issues

### "Module not found"
```bash
pip install flask flask-cors
```

### "Port already in use"
Change port in app.py line 283:
```python
app.run(debug=True, port=5001)
```

### "Template not found"
Make sure `index.html` is in `templates/` folder

---

## 📞 Need Help?

1. Check if Flask server is running
2. Open browser console (F12) for errors
3. Verify folder structure matches above
4. Ensure Python 3.7+ is installed

---

## 🎉 You're All Set!

Your Student Performance Analysis System is ready to use!