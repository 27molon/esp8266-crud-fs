const baseURL = 'http://' + window.location.hostname;

document.addEventListener('DOMContentLoaded', (event) => {
    loadEntries();
});

function createEntry() {
    const form = document.getElementById('createForm');
    const formData = new FormData(form);
    fetch(baseURL + '/create', {
        method: 'POST',
        body: new URLSearchParams(formData)
    }).then(response => response.text())
      .then(data => {
          alert(data);
          loadEntries();
      });
}

function loadEntries() {
    fetch(baseURL + '/read')
        .then(response => response.text())
        .then(data => {
            const rows = data.trim().split('\n');
            const tableBody = document.getElementById('entriesTable').querySelector('tbody');
            tableBody.innerHTML = '';

            rows.forEach(row => {
                const columns = row.split(',');
                const tr = document.createElement('tr');

                columns.forEach(column => {
                    const td = document.createElement('td');
                    td.textContent = column;
                    tr.appendChild(td);
                });

                const actionTd = document.createElement('td');
                const deleteButton = document.createElement('button');
                deleteButton.textContent = 'Delete';
                deleteButton.onclick = () => deleteEntry(columns[0], columns[1], columns[2]);
                actionTd.appendChild(deleteButton);

                const updateButton = document.createElement('button');
                updateButton.textContent = 'Update';
                updateButton.onclick = () => updateEntryPrompt(columns[0], columns[1], columns[2]);
                actionTd.appendChild(updateButton);

                tr.appendChild(actionTd);
                tableBody.appendChild(tr);
            });
        });
}

function deleteEntry(name, lastname, age) {
    fetch(baseURL + '/delete', {
        method: 'POST',
        body: new URLSearchParams({ name, lastname, age })
    }).then(response => response.text())
      .then(data => {
          alert(data);
          loadEntries();
      });
}

function updateEntryPrompt(oldName, oldLastname, oldAge) {
    const newName = prompt("New Name:", oldName);
    const newLastname = prompt("New Lastname:", oldLastname);
    const newAge = prompt("New Age:", oldAge);

    if (newName && newLastname && newAge) {
        updateEntry(oldName, oldLastname, oldAge, newName, newLastname, newAge);
    }
}

function updateEntry(oldName, oldLastname, oldAge, newName, newLastname, newAge) {
    fetch(baseURL + '/update', {
        method: 'POST',
        body: new URLSearchParams({
            oldName, oldLastname, oldAge, newName, newLastname, newAge
        })
    }).then(response => response.text())
      .then(data => {
          alert(data);
          loadEntries();
      });
}

function changeWiFi() {
    const form = document.getElementById('wifiForm');
    const formData = new FormData(form);
    fetch(baseURL + '/changeWiFi', {
        method: 'POST',
        body: new URLSearchParams(formData)
    }).then(response => response.text())
      .then(data => {
          alert(data);
          location.reload();
      });
}

function toggleTheme() {
    document.body.classList.toggle('dark-mode');
}
