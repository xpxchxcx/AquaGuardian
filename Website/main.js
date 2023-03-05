window.onload = function() {
  var expandIcon = document.getElementById("expand-icon");
  var navColumn = document.getElementById("nav-column");
  navColumn.style.transform = "translateX(-100%)";
  expandIcon.addEventListener("click", toggleExpand);
}

function toggleExpand() {
  var navColumn = document.getElementById("nav-column");
  if (navColumn.classList.contains("expanded")) {
    navColumn.style.transform = "translateX(-100%)";
    navColumn.classList.remove("expanded");
  } else {
    navColumn.style.transform = "translateX(0)";
    navColumn.classList.add("expanded");
  }
}

function displaySection(sectionId) {
  var sections = document.querySelectorAll("section");
  for (var i = 0; i < sections.length; i++) {
    sections[i].style.display = "none";
  }

  var section = document.getElementById(sectionId);
  section.style.display = "block";
}
