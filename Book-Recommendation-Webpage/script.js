const apiUrl = 'https://www.googleapis.com/books/v1/volumes?q=';

// DOM Elements
const searchInput = document.querySelector('input');
const searchButton = document.querySelector('.btnsrc');
const bookContainer = document.createElement('div');
document.body.appendChild(bookContainer);
bookContainer.classList.add('book-container');

// Event Listener for Search
searchButton.addEventListener('click', () => {
  const query = searchInput.value.trim();
  if (query) {
    fetchBooks(query);
  } else {
    alert('Please enter a search term.');
  }
});

// Fetch Books from API
function fetchBooks(query) {
  fetch(`${apiUrl}${query}`)
    .then(response => response.json())
    .then(data => {
      displayBooks(data.items);
    })
    .catch(error => {
      console.error('Error fetching books:', error);
      alert('Failed to fetch books. Please try again.');
    });
}

// Display Book Cards
function displayBooks(books) {
  bookContainer.innerHTML = ''; // Clear previous results
  books.forEach(book => {
    const bookCard = document.createElement('div');
    bookCard.classList.add('book-card');

    const thumbnail = book.volumeInfo.imageLinks?.thumbnail || 'placeholder.jpg';
    const title = book.volumeInfo.title || 'No Title';
    const authors = book.volumeInfo.authors?.join(', ') || 'Unknown Author';
    const description = book.volumeInfo.description || 'No Description Available';

    bookCard.innerHTML = `
      <img src="${thumbnail}" alt="${title}" class="book-image">
      <h3 class="book-title">${title}</h3>
      <p class="book-author">${authors}</p>
      <button class="details-button" data-id="${book.id}">View Details</button>
    `;

    bookContainer.appendChild(bookCard);

    // Add Event Listener for "View Details" Button
    bookCard.querySelector('.details-button').addEventListener('click', () => {
      displayBookDetails(book);
    });
  });
}

// Display Detailed Information
function displayBookDetails(book) {
  const detailsHtml = `
    <div class="book-details">
      <h2>${book.volumeInfo.title}</h2>
      <p><strong>Authors:</strong> ${book.volumeInfo.authors?.join(', ') || 'Unknown'}</p>
      <p><strong>Description:</strong> ${book.volumeInfo.description || 'Not available.'}</p>
      <p><strong>Categories:</strong> ${book.volumeInfo.categories?.join(', ') || 'Not available.'}</p>
      <a href="${book.volumeInfo.previewLink}" target="_blank">Preview or Buy</a>
    </div>
  `;
  bookContainer.innerHTML = detailsHtml;
}
document.addEventListener("DOMContentLoaded", () => {
  console.log("DOM is loaded!"); // Check if this logs to the console

  const menuToggle = document.querySelector(".menu-toggle");
  const navLinks = document.querySelector("nav ul");

  menuToggle.addEventListener("click", () => {
    console.log("Menu toggle clicked!"); // Check if this logs when you click
    navLinks.classList.toggle("active");
  });
});