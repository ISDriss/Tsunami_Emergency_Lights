// Get the modal
var modal = document.getElementById("myModal");

// Get the <span> element that closes the modal
var span = document.getElementsByClassName("close")[0];

// When the user clicks on <span> (x), close the modal
span.onclick = function() {
    modal.style.display = "none";
    document.body.style.overflow = "auto"; // Enable scrolling
}

// When the user clicks anywhere outside of the modal, close it
window.onclick = function(event) {
    if (event.target == modal) {
        modal.style.display = "none";
        document.body.style.overflow = "auto"; // Enable scrolling
    }
}

// Function to open the modal with specific content
function openModal(title, content) {
    document.getElementById("modal-text").innerHTML = "<h2>" + title + "</h2><p>" + content + "</p>";
    modal.style.display = "block";
    document.body.style.overflow = "hidden"; // Disable scrolling
}
