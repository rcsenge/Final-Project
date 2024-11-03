## Event Management Program

This program is designed to organize events for easy viewing. Users can add new events, modify existing ones, or delete them. It features a menu-driven interface where users can choose from several options upon startup:

1. Add a record
2. Modify a record
3. Delete a record
4. Write to a file
5. Read from a file
6. Exit

After executing a task, users can select another option until they choose to exit the program. If an invalid input is given, an error message is displayed, and the menu is shown again.

### Features

- **Adding Events**: Users can specify a date (year.month.day), time (hour:minute), location, and a comment. All fields are mandatory.
  
- **Modifying Events**: Users can modify any detail of an existing event by entering the event name. The program lists the modifiable fields and prompts the user to choose which one to change.

- **Listing Events**: Users can view events by day, week, or month by entering the corresponding number. Events are displayed in chronological order.

- **Searching for Events**: Users can search for events by name, and the program will list the details if found, or notify the user if not.

- **Deleting Records**: To delete an event, users provide its name and must confirm the deletion.

- **File I/O**: Users can save the event database to a file of their choosing. If the specified file exists, data is written to it; if not, a new file is created. Events are saved in the format: `name, date (year.month.day), time (hour:minute), location (country, city, street name house number), comment`.

- **Loading Data**: The program can read from an existing file, provided the data is in the correct format. Successful loading is confirmed, along with any errors that occur.

### Memory Management

The program utilizes dynamic memory management to efficiently handle data storage and ensure optimal memory usage.
