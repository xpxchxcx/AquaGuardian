// Get the email and password input elements from the HTML form
const emailInput = document.getElementById("email");
const passwordInput = document.getElementById("password");

// Function to sign in the user with email and password
function signIn() {
  const email = emailInput.value;
  const password = passwordInput.value;

  firebase.auth().signInWithEmailAndPassword(email, password)
  .then((userCredential) => {
    // If sign-in is successful, redirect to the hidden HTML page
    window.location.href = "hidden-page.html";
  })
  .catch((error) => {
    // If sign-in fails, show an error message
    const errorCode = error.code;
    const errorMessage = error.message;

    if (errorCode === "auth/invalid-email") {
      alert("Invalid email address.");
    } else if (errorCode === "auth/wrong-password") {
      alert("Incorrect password.");
    } else if (errorCode === "auth/user-not-found") {
      alert("User not found.");
    } else {
      alert("Error: " + errorMessage);
    }
  });

}

  