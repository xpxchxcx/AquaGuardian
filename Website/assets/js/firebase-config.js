// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getAnalytics } from "firebase/analytics";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "AIzaSyDYe5fJgfC9J1NKhkdV-3gMlfM4qIlnDkM",
  authDomain: "gsc-team-1-water.firebaseapp.com",
  databaseURL: "https://gsc-team-1-water-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "gsc-team-1-water",
  storageBucket: "gsc-team-1-water.appspot.com",
  messagingSenderId: "613542511794",
  appId: "1:613542511794:web:673ea7840bf8f0e4f935ab",
  measurementId: "G-4Q9J97G9JK"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const analytics = getAnalytics(app);
  