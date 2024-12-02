# Deadline Planner

The **Deadline Planner** application helps users track and manage their events effectively. Users can create, modify, delete, list events within a specific time frame, search for events, and export or import event data to/from a file.

---

## **Main Features**

### **1. Event Management**
- Users can add, delete, and modify events.
- Events include the following details:
  - **Name**
  - **Date**
  - **Time**
  - **Location**
  - **Notes**
- Easily manage event details through simple menu options.

### **2. Event Search**
- Search for events by name.
- The program lists all matching results, even if there are multiple matches.

### **3. Event Filtering**
- Filter events by:
  - **Day**
  - **Week**
  - **Month**
- Specify a date, and the program will list events relative to the chosen time frame.

### **4. Saving and Loading Event Database**
- Save events to a `.txt` file for backup or transfer.
- Load events from a file when the program starts (available only at startup).

---

## **How to Use**

### **1. Adding an Event**
1. Select the **"Add Event"** option.
2. Enter the event details:
   - **Name**
   - **Date** (format: `YYYY. MM. DD.`)
   - **Time** (format: `HH:MM`)
   - **Location**
   - **Notes**
3. The event is saved in the database and immediately available.

### **2. Deleting an Event**
1. Select the **"Delete"** option.
2. Provide the event name.
3. If multiple events share the same name, the program lists them with serial numbers.
4. Select the event to delete by its serial number.

### **3. Modifying an Event**
1. Select the **"Modify"** option.
2. Provide the event name.
3. If multiple events share the same name, the program lists them with serial numbers.
4. Select the event to modify by its serial number.
5. Update one or more event fields.

### **4. Searching for Events**
1. Select the **"Search"** option.
2. Provide the event name.
3. The program lists all matching events.

### **5. Filtering Events by Time Period**
1. Choose to filter by **day**, **week**, or **month**.
2. Specify a date to define the desired time frame.
3. The program lists events chronologically within the selected period.

### **6. Saving Events to a File**
1. Select the **"Save to File"** option.
2. Specify the file name and location.
3. The program saves the events in a `.txt` file, e.g.:

   ```
   1. Event:
   Name: Hairdresser
   Date: 2024. 12. 01.
   Time: 12:12
   Location: Budapest, Falk Miksa u. 5.
   Notes: Arrive 10 minutes early.

   2. Event:
   Name: Programming Assignment Deadline
   Date: 2024. 11. 24.
   Time: 23:59
   Location: Online
   Notes: Don't forget the programming and user documentation!

   3. Event:
   Name: BSZ Exam
   Date: 2024. 11. 26.
   Time: 18:00
   Location: BME E Building
   Notes: Focus on Gaussian elimination.
   ```

### **7. Loading Events from a File**
- Upon program startup, use the **"Load File"** option.
- Specify a `.txt` file containing event data.
- Events are loaded into the database for further management.
