document.addEventListener('DOMContentLoaded', function() {
  document.querySelectorAll('.btn-principal, .btn-secundario').forEach(function(btn) {
    btn.addEventListener('click', function() {
      alert('Em breve disponível para compra!');
    });
  });

  // Animação ao rolar para as seções
  const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
      if (entry.isIntersecting) {
        entry.target.style.animationPlayState = 'running';
        observer.unobserve(entry.target);
      }
    });
  }, { threshold: 0.1 });

  document.querySelectorAll('.section-animate').forEach(section => {
    section.style.animationPlayState = 'paused';
    observer.observe(section);
  });

  // Mostra apenas um depoimento por vez
  const depoimentos = document.querySelectorAll('#depoimentos-carousel blockquote');
  let depoimentoIdx = 0;

  function showDepoimento(idx) {
    depoimentos.forEach((el, i) => {
      el.classList.toggle('active', i === idx);
    });
    // Indicador opcional
    let indicator = document.getElementById('depoimento-indicator');
    if (!indicator) {
      indicator = document.createElement('div');
      indicator.id = 'depoimento-indicator';
      indicator.style.margin = '1rem 0';
      indicator.style.fontWeight = 'bold';
      indicator.style.color = '#15803d';
      const nav = document.querySelector('.depoimentos-nav');
      if (nav) nav.parentNode.insertBefore(indicator, nav);
    }
    indicator.textContent = `Depoimento ${idx + 1} de ${depoimentos.length}`;
  }

  // Inicializa mostrando o primeiro depoimento
  if (depoimentos.length > 0) {
    showDepoimento(depoimentoIdx);
  }
});

// Header fixo ao rolar
(function() {
  const header = document.querySelector('.header-trapezio');
  if (!header) return;
  let fixo = false;
  window.addEventListener('scroll', function() {
    if (window.scrollY > 40) {
      if (!fixo) {
        header.classList.add('fixo');
        fixo = true;
      }
    } else {
      if (fixo) {
        header.classList.remove('fixo');
        fixo = false;
      }
    }
  });
})();
